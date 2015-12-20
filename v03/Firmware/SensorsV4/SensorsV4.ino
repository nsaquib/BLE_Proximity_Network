/*
 * SensorsV4.ino - Network protocol for PrNet nodes.
 * Created by Dwyane George, November 2, 2015.
 * Social Computing Group, MIT Media Lab
 */

#include <PrNetRomManager.h>
#include <RFduinoBLE.h>
#include <RFduinoGZLL.h>
#include <stdlib.h>
#include <Time.h>

#define TX_POWER_LEVEL 4
#define BLE_AD_INTERVAL 2000
#define PAGES_TO_TRANSFER 50
#define HBA 0x000
// Configuration Parameters
#define MAX_DEVICES 5
#define START_HOUR 3
#define START_MINUTE 43
#define END_HOUR 3
#define END_MINUTE 44
#define HOST_LOOP_TIME 100
#define HOST_LOOPS 1
#define DEVICE_LOOP_TIME 100

// Unique device ID
const int deviceID = 0;
// Device loops
const int DEVICE_LOOPS = (DEVICE_LOOP_TIME == 0) ? 0 : (HOST_LOOP_TIME*HOST_LOOPS)*(MAX_DEVICES-1)/(DEVICE_LOOP_TIME);
// Initial device role
device_t deviceRole = HOST;
// RSSI total and count for each device
int rssiTotal[MAX_DEVICES];
int rssiCount[MAX_DEVICES];
// Collect samples flag
boolean collectSamples = false;
// Time
Time timer;
// ROM Manager for writing to flash ROM
PrNetRomManager romManager;
// ROM Manager for reading flash ROM and sending through BLE
PrNetRomManager romManager2;
// Counter for current ROM page row
int rowCounter = 0;
// Flag to transfer data to cell phone app
boolean transferFlag = false;
// Counter for current page to write to
int pageCounter = STORAGE_FLASH_PAGE;
// Update start time
char month[2];
char day[2];
char year[2];
char weekday[1];
char hour[2];
char minute[2];
char second[2];
char ms[2];

int offset = 0;

/*
 * Erases ROM memory and sets radio parameters
 */
void setup() {
  Serial.begin(9600);
  RFduinoGZLL.txPowerLevel = TX_POWER_LEVEL;
  RFduinoGZLL.hostBaseAddress = HBA;
  /*String deviceIDString = String(deviceID);
  char BLE_NAME[2];
  deviceIDString.toCharArray(BLE_NAME, deviceIDString.length() + 1);
  BLE_NAME[0] = char(32);
  BLE_NAME[1] = char(33);
  int id = snprintf(BLE_NAME, 10, "%d", deviceID);
  RFduinoBLE.advertisementData = BLE_NAME;*/
  RFduinoBLE.deviceName = "0";
  if (deviceRole == HOST) {
    setupHost();
  } else {
    setupDevice();
  }
}

/*
 * Switches GZLL stack to host mode
 */
void setupHost() {
  deviceRole = HOST;
  RFduinoGZLL.end();
  RFduinoGZLL.begin(deviceRole);
}

/*
 * Switches GZLL stack to device mode
 */
void setupDevice() {
  deviceRole = (device_t) (deviceID % 8);
  RFduinoGZLL.end();
  RFduinoGZLL.begin(deviceRole);
}

/*
 * Checks that time is set, if in data collection period, collects data, sleeps device otherwise
 */
void loop() {
  if (!timer.isTimeSet) {
    waitForTime();
    eraseROM();
  } else {
    if (!timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE + offset)) {
      sleepUntilStartTime();
      if (transferFlag) {
        startTransfer();
      }
    } else {
      if (deviceRole == HOST) {
        loopHost();
      } else {
        loopDevice();
      }
    }
  }
}

/*
 * Collects data samples HOST_LOOPS time for HOST_LOOP_TIME milliseconds
 */
void loopHost() {
  Serial.println("HOST");
  writeTimeROMRow();
  timer.displayDateTime();
  if (timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE + offset)) {
    for (int i = 0; i < HOST_LOOPS; i++) {
      if (!timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE + offset)) {
        sleepUntilStartTime();
      }
      collectSamplesFromDevices();
      updateROMTable();
    }
    setupDevice();
  }
}

/*
 * Polls host device for DEVICE_LOOPS times with DEVICE_LOOP_TIME delay between polls
 */
void loopDevice() {
  Serial.println("DEVICE");
  timer.displayDateTime();
  if (timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE + offset)) {
    for (int i = 0; i < DEVICE_LOOPS; i++) {
      if (!timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE + offset)) {
        sleepUntilStartTime();
      }
      timer.updateTime();
      timer.delayTime(DEVICE_LOOP_TIME - ((timer.currentTime.seconds*1000 + timer.currentTime.ms) % DEVICE_LOOP_TIME));
      pollHost();
    }
    setupHost();
  }
}

////////// Host Functions //////////

/*
 * Turns on data collection flag for HOST_LOOP_TIME milliseconds
 */
void collectSamplesFromDevices() {
  collectSamples = true;
  timer.updateTime();
  timer.delayTime(HOST_LOOP_TIME - ((timer.currentTime.seconds*1000 + timer.currentTime.ms) % HOST_LOOP_TIME));
  collectSamples = false;
}

////////// Device Functions //////////

/*
 * Sends the deviceID to host devices
 */
void pollHost() {
  if (deviceRole != HOST) {
    RFduinoGZLL.sendToHost(deviceID);
  }
}

////////// Callback Functions //////////

/*
 * Collects RSSI values from incoming data transmissions
 */
void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
  // If collecting samples, update the RSSI total and count
  if (deviceRole == HOST && collectSamples && (int) data[0] >= 0 && (int) data[0] < MAX_DEVICES) {
    rssiTotal[(int) data[0]] += rssi;
    rssiCount[(int) data[0]]++;
  }
}

////////// Time Functions //////////

/*
 * Devices device until time is set
 */
void waitForTime() {
  RFduinoGZLL.end();
  RFduinoBLE.advertisementInterval = BLE_AD_INTERVAL;
  RFduinoBLE.begin();
  Serial.println("Waiting for time...");
  while (!timer.isTimeSet) {
    timer.delayTime(5);
    if (transferFlag) {
      startTransfer();
    }
  }
  RFduinoBLE.end();
}

////////// Sleep Functions //////////

/*
 * Disables battery intensive facilities and sleeps device for given amount of milliseconds
 */
void sleepDevice(int milliseconds) {
  offset += 2;
  writeTimeROMRow();
  if (rowCounter >= MAX_ROWS) {
    writePage();
  } else {
    writePartialPage();
  }
  RFduinoGZLL.end();
  Serial.end();
  RFduino_ULPDelay(60000);
  //RFduino_ULPDelay(milliseconds);
  writeTimeROMRow();
  deviceRole = HOST;
  Serial.begin(9600);
  RFduinoGZLL.begin(deviceRole);
}

/*
 * Sleeps device until START_HOUR:START_MINUTE plus offset to shift each device's host time
 */
void sleepUntilStartTime() {
  int delayTime = timer.getTimeUntilStartTime(START_HOUR, START_MINUTE);
  // Add additional offset to shift each device's initial start time
  delayTime += HOST_LOOP_TIME * HOST_LOOPS * deviceID;
  Serial.print("Offset: ");
  Serial.println(HOST_LOOP_TIME * HOST_LOOPS * deviceID);
  sleepDevice(delayTime);
}

////////// ROM Code //////////

/*  
 * Stores deviceID (I), RSSI (R), and time (T) as I,IRR,TTT,TTT with maximum value of 4,294,967,295
 */
void updateROMTable() {
  for (int i = 0; i < MAX_DEVICES; i++) {
    int rssiAverage = (rssiCount[i] == 0) ? -128 : rssiTotal[i] / rssiCount[i];
    Serial.print(i);
    Serial.print(",");
    Serial.print(rssiAverage);
    Serial.print(",");
    Serial.println(rssiCount[i]);
    //if (rssiAverage > -100) {
      if (rowCounter >= MAX_ROWS) {
        writePage();
      }
      int data = (millis()/1000) % 1000000;
      data += abs(rssiAverage % 100) * 1000000;
      data += (i % 42) * 100000000;
      romManager.table.data[rowCounter] = data;
      rowCounter++;
    //}
    rssiTotal[i] = 0;
    rssiCount[i] = 0;
  }
}

/*
 * Write a timestamp row HH:MM:SS to the ROM table
 */
void writeTimeROMRow() {
  if (rowCounter >= MAX_ROWS) {
    writePage();
  }
  timer.updateTime();
  int data = timer.currentTime.seconds;
  data += timer.currentTime.minutes * 100;
  data += timer.currentTime.hours * 10000;
  romManager.table.data[rowCounter] = data;
  rowCounter++;
}

/*
 * Persists the table to nonvolatile ROM memory
 */
void writePage() {
  int success = romManager.writePage(STORAGE_FLASH_PAGE - romManager.pagecounter, romManager.table);
  Serial.println(success);
  rowCounter = 0;
}

/*
 * Persists a partially filled table to nonvolatile ROM memory
 */
void writePartialPage() {
  for (int i = rowCounter; i < MAX_ROWS; i++) {
    romManager.table.data[i] = -1;
  }
  int success = romManager.writePartialPage(STORAGE_FLASH_PAGE - romManager.pagecounter, romManager.table);
  Serial.println(success);
  rowCounter++;
}

/*
 * Erases all existing ROM data
 */
void eraseROM() {
  for (int i = STORAGE_FLASH_PAGE; i > STORAGE_FLASH_PAGE - 180; i--) {
    romManager.erasePage(i);
  }
  Serial.println("Erased ROM");
}

////////// App Integration Code //////////

/*
 * Sets device parameters through BLE
 */
void RFduinoBLE_onReceive(char *data, int len) {
  if (data[0]) {
    if (data[0] == '>') {
      timer.initialMillis = millis();
      hour[0] = data[1];
      hour[1] = data[2];
      minute[0] = data[4];
      minute[1] = data[5];
      second[0] = data[7];
      second[1] = data[8];
      ms[0] = data[10];
      ms[1] = data[11];
      /*month[0] = data[1];
      month[1] = data[2];
      day[0] = data[3];
      day[1] = data[4];
      year[0] = data[5];
      year[1] = data[6];
      weekday[0] = data[7];
      hour[0] = data[8];
      hour[1] = data[9];
      minute[0] = data[10];
      minute[1] = data[11];
      second[0] = data[12];
      second[1] = data[13];
      ms[0] = data[14];
      ms[1] = data[15];*/
      RFduinoBLE.send('>');
      timer.setInitialTime(0, 0, 0, 0, atoi(hour), atoi(minute), atoi(second), atoi(ms));
      //timer.setInitialTime(atoi(month), atoi(day), atoi(year), atoi(weekday), atoi(hour), atoi(minute), atoi(second), atoi(ms));
      timer.isTimeSet = true;
      writeTimeROMRow();
    } else {
      RFduinoBLE.send(1);
      transferFlag = true;
    }
  } else {
    RFduinoBLE.send(0);
    transferFlag = false;
  }
}

/*
 * Transfers device ROM to BLE-enabled device
 */
void startTransfer() {
  while (transferFlag) {
    romManager2.loadPage(pageCounter);
    Serial.print("Sending page ");
    Serial.println(pageCounter);
    // Generate the next packet
    for (int i = 0; i < MAX_ROWS; i++) {
      char space = ' ';
      char dataBuffer[10];
      sprintf(dataBuffer, "%d", romManager2.table.data[i]);
      while (!RFduinoBLE.send(dataBuffer, 10));
      timer.delayTime(5);
      while (!RFduinoBLE.send('|'));
    }
    while (!RFduinoBLE.send('#'));
    timer.delayTime(200);
    pageCounter--;
    if (pageCounter < STORAGE_FLASH_PAGE - PAGES_TO_TRANSFER - 1) {
      Serial.println("Transfer complete");
      transferFlag = false;
      RFduinoBLE.send(0);
    }
  }
}
