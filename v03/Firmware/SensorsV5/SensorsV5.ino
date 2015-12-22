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
#define BAUD_RATE 9600
// Configuration Parameters
#define MAX_DEVICES 4
#define START_HOUR 21
#define START_MINUTE 51
#define END_HOUR 22
#define END_MINUTE 51
#define HOST_LOOP_TIME 500
#define HOST_LOOPS 1
#define DEVICE_LOOP_TIME 100

// Unique device ID
const int deviceID = 3;
// Device BLE name
char deviceBLEName[2];
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

int offset = 0; // Debug variable

/*
 * Erases ROM memory and sets radio parameters
 */
void setup() {
  Serial.begin(BAUD_RATE);
  deviceBLEName[0] = (deviceID < 10) ? deviceID + '0' : ((deviceID - (deviceID % 10)) / 10) + '0';
  deviceBLEName[1] = (deviceID < 10) ? 0 : (deviceID % 10) + '0';
  RFduinoGZLL.txPowerLevel = TX_POWER_LEVEL;
  RFduinoGZLL.hostBaseAddress = HBA;
  RFduinoBLE.deviceName = deviceBLEName;
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
//  RFduino_ULPDelay(60000);
  RFduino_ULPDelay(milliseconds);
}

/*
 * Sleeps device until START_HOUR:START_MINUTE plus offset to shift each device's host time
 */
void sleepUntilStartTime() {
//  offset += 2;
  RFduinoGZLL.end();
  writeTimeROMRow();
  if (rowCounter >= MAX_ROWS) {
    writePage();
  } else {
    writePartialPage();
  }
  struct sleepTime sleepTime = timer.getTimeUntilStartTime(START_HOUR, START_MINUTE);
  // Calculate time to startHour:startMinute by converting higher order time units to ms
  int sleepTimeMillis = sleepTime.ms + (1000 * sleepTime.seconds) + (60000 * sleepTime.minutes) + (3600000 * sleepTime.hours) + (86400000 * sleepTime.days);
  // Add additional offset to shift each device's initial start time
  sleepTimeMillis += HOST_LOOP_TIME * HOST_LOOPS * deviceID;
  Serial.print("Sleeping for ");
  Serial.print(sleepTime.days);
  Serial.print(":");
  Serial.print(sleepTime.hours);
  Serial.print(":");
  Serial.print(sleepTime.minutes);
  Serial.print(":");
  Serial.print(sleepTime.seconds);
  Serial.print(":");
  Serial.println(sleepTime.ms);
  Serial.print("Offset: ");
  Serial.println(HOST_LOOP_TIME * HOST_LOOPS * deviceID);
  Serial.end();

  RFduinoBLE.begin();
  sendSleepTime(sleepTime);
  RFduinoBLE.end();
  
  sleepDevice(sleepTimeMillis);
  writeTimeROMRow();
  deviceRole = HOST;
  Serial.begin(BAUD_RATE);
  RFduinoGZLL.begin(deviceRole);
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
    if (rssiAverage > -100) {
      if (rowCounter >= MAX_ROWS) {
        writePage();
      }
      int data = (millis()/1000) % 1000000;
      data += abs(rssiAverage % 100) * 1000000;
      data += (i % 42) * 100000000;
      romManager.table.data[rowCounter] = data;
      rowCounter++;
    }
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
 * Sets device parameters with format MMddyyEHHmmssSSS through BLE
 */
void RFduinoBLE_onReceive(char *data, int len) {
  if (data[0]) {
    if (data[0] == '>' && data[16] && !timer.isTimeSet) {
      timer.initialMillis = millis();
      timer.setInitialTime(
        (data[1] - '0') * 10 + (data[2] - '0'),                             // Month
        (data[3] - '0') * 10 + (data[4] - '0'),                             // Date
        (data[5] - '0') * 10 + (data[6] - '0'),                             // Year
        (data[7] - '0'),                                                    // Day
        (data[8] - '0') * 10 + (data[9] - '0'),                             // Hour
        (data[10] - '0') * 10 + (data[11] - '0'),                           // Minute
        (data[12] - '0') * 10 + (data[13] - '0'),                           // Second
        (data[14] - '0') * 100 + (data[15] - '0') * 10 + (data[16] - '0')); // Millisecond
      while (!RFduinoBLE.send('>'));
      timer.displayDateTime();
      writeTimeROMRow();
      sendCurrentTime();
    } else if (data[0] == '#') {
      while (!RFduinoBLE.send(1));
      transferFlag = true;
    }
  } else {
    while (!RFduinoBLE.send(0));
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
      char dataBuffer[10];
      sprintf(dataBuffer, "%d", romManager2.table.data[i]);
      while (!RFduinoBLE.send(dataBuffer, 10));
      while (!RFduinoBLE.send('|'));
    }
    while (!RFduinoBLE.send('#'));
    timer.delayTime(200);
    pageCounter--;
    if (pageCounter < STORAGE_FLASH_PAGE - PAGES_TO_TRANSFER) {
      Serial.println("Data transfer complete");
      transferFlag = false;
      RFduinoBLE.send(0);
    }
  }
}

void sendCurrentTime() {
//  timer.updateTime();
//  while (!RFduinoBLE.send('>'));
//  while (!RFduinoBLE.send(timer.currentTime.month));
//  while (!RFduinoBLE.send(timer.currentTime.date));
//  while (!RFduinoBLE.send(timer.currentTime.year));
//  while (!RFduinoBLE.send(timer.currentTime.day));
//  while (!RFduinoBLE.send(timer.currentTime.hours));
//  while (!RFduinoBLE.send(timer.currentTime.minutes));
//  while (!RFduinoBLE.send(timer.currentTime.seconds));
//  while (!RFduinoBLE.send(timer.currentTime.ms));
}

void sendSleepTime(struct sleepTime sleepTime) {
//  while (!RFduinoBLE.send(sleepTime.days));
//  while (!RFduinoBLE.send(sleepTime.hours));
//  while (!RFduinoBLE.send(sleepTime.minutes));
//  while (!RFduinoBLE.send(sleepTime.seconds));
//  while (!RFduinoBLE.send(sleepTime.ms));
}

