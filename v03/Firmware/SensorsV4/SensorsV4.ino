/*
 * SensorsV4.ino - Network protocol for PrNet nodes.
 * Created by Dwyane George, November 2, 2015.
 * Social Computing Group, MIT Media Lab
 */

#define TX_POWER_LEVEL 4
#define BLE_AD_INTERVAL 2000
#define PAGES_TO_TRANSFER 50
#define HBA 0x000
// Configuration Parameters
#define MAX_DEVICES 3
#define START_HOUR 15
#define START_MINUTE 1
#define END_HOUR 23
#define END_MINUTE 0
#define HOST_LOOP_TIME 1000
#define HOST_LOOPS 1
#define DEVICE_LOOP_TIME 100
// Flag for Serial monitor debug output
#define VERBOSE_FLAG true

#include <PrNetRomManager.h>
#include <RFduinoBLE.h>
#include <RFduinoGZLL.h>
#include <stdlib.h>
#include <Time.h>

// Unique device ID
const int deviceID = 2;
// Device loops
const int DEVICE_LOOPS = (DEVICE_LOOP_TIME == 0) ? 0 : (HOST_LOOP_TIME*HOST_LOOPS)*(MAX_DEVICES-1)/(DEVICE_LOOP_TIME);
// Pin for the green LED
const int greenLED = 3;
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

void setup() {
  pinMode(greenLED, OUTPUT);
  RFduinoGZLL.txPowerLevel = TX_POWER_LEVEL;

  //String deviceIDString = String(deviceID);
  //char BLE_NAME[deviceIDString.length() + 1];
  //deviceIDString.toCharArray(BLE_NAME, deviceIDString.length() + 1);
  //BLE_NAME[0] = '2';
  //BLE_NAME[1] = '3';
  //int id = snprintf(BLE_NAME, 10, "%d", deviceID);
  RFduinoBLE.deviceName = "2"; // BLE_NAME;
  //RFduinoBLE.advertisementData = BLE_NAME;
  
  RFduinoGZLL.hostBaseAddress = HBA;
  if (VERBOSE_FLAG) {
    Serial.begin(9600);
  }
  if (deviceRole == HOST) {
    setupHost();
  } else {
    setupDevice();
  }
}

void setupHost() {
  deviceRole = HOST;
  RFduinoGZLL.end();
  RFduinoGZLL.begin(deviceRole);
}

void setupDevice() {
  deviceRole = (device_t) (deviceID % 8);
  RFduinoGZLL.end();
  RFduinoGZLL.begin(deviceRole);
}

void loop() {
  if (!timer.isTimeSet) {
    waitForTime();
  } else {
    if (!timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE)) {
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

void loopHost() {
  Serial.println("HOST");
  if (timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE)) {
    for (int i = 0; i < HOST_LOOPS; i++) {
      timer.displayDateTime();
      collectSamplesFromDevices();
      updateROMTable();
    }
    setupDevice();
  }
}

void loopDevice() {
  Serial.println("DEVICE");
  if (timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE)) {
    for (int i = 0; i < DEVICE_LOOPS; i++) {
      timer.displayDateTime();
      //timer.updateTime();
      timer.delayTime(DEVICE_LOOP_TIME - ((timer.currentTime.seconds*1000 + timer.currentTime.ms) % DEVICE_LOOP_TIME));
      pollHost();
    }
    setupHost();
  }
}

////////// Host Functions //////////

void collectSamplesFromDevices() {
  collectSamples = true;
  timer.updateTime();
  timer.delayTime(HOST_LOOP_TIME - ((timer.currentTime.seconds*1000 + timer.currentTime.ms) % HOST_LOOP_TIME));
  collectSamples = false;
}

////////// Device Functions //////////

void pollHost() {
  if (deviceRole != HOST) {
    RFduinoGZLL.sendToHost(deviceID);
  }
}

////////// Callback Functions //////////

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
  // If collecting samples, update the RSSI total and count
  if (deviceRole == HOST && collectSamples && (int) data[0] >= 0 && (int) data[0] < MAX_DEVICES) {
    rssiTotal[(int) data[0]] += rssi;
    rssiCount[(int) data[0]]++;
  }
}

////////// Time Functions //////////

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

void sleepDevice(int milliseconds) {
  RFduinoGZLL.end();
  RFduino_ULPDelay(milliseconds);
  timer.updateTime();
  RFduinoGZLL.begin(deviceRole);
}

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
 *  Store deviceID (I), RSSI (R), and time (T) as I,IRR,TTT,TTT
 *  Maximum value is 4,294,967,295
 *  Maximum deviceID is 41
 *  Maximum RSSI is 99 (-99)
 *  Maximum time is 999,999 seconds
 */
void updateROMTable() {
  // Update rows for ROM table and clear collected values
  for (int i = 0; i < MAX_DEVICES; i++) {
    int rssiAverage = (rssiCount[i] == 0) ? -128 : rssiTotal[i] / rssiCount[i];
    /*Serial.print(i);
    Serial.print(",");
    Serial.print(rssiAverage);
    Serial.print(",");*/
    Serial.println(rssiCount[i]);
    if (rssiAverage > -100) {
      if (rowCounter >= MAX_ROWS) {
        writePage();
      }
      int data = millis()/1000 % 1000000;
      data += (abs(rssiAverage % 100)) * 1000000;
      data += (i % 42) * 100000000;
      romManager.table.data[rowCounter] = data;
      rowCounter++;
    }
    rssiTotal[i] = 0;
    rssiCount[i] = 0;
  }
}

void writePage() {
  // Erase existing data in the current rom page and write page to memory
  romManager.erasePage(STORAGE_FLASH_PAGE - romManager.pagecounter);
  int success = romManager.writePage(STORAGE_FLASH_PAGE - romManager.pagecounter, romManager.table);
  Serial.println(success);
  rowCounter = 0;
}

////////// App Integration Code //////////

void RFduinoBLE_onReceive(char *data, int len) {
  // Check if the first byte exists
  if (data[0]) {
    if (data[0] == '>') {
      // Update start time
      char month[2];
      char day[2];
      char year[2];
      char weekday[1];
      char hour[2];
      char minute[2];
      char second[2];
      char ms[2];
      // Remove below lines
      hour[0] = data[1];
      hour[1] = data[2];
      minute[0] = data[4];
      minute[1] = data[5];
      second[0] = data[7];
      second[1] = data[8];
      ms[0] = data[10];
      ms[1] = data[11];
      // Uncomment following lines
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
      // TODO: get date from app and set date
      timer.setInitialTime(0, 0, 0, 0, atoi(hour), atoi(minute), atoi(second), atoi(ms));
      timer.initialMillis = millis();
      timer.delayTime(HOST_LOOP_TIME - ((timer.currentTime.seconds*1000 + timer.currentTime.ms) % HOST_LOOP_TIME));
      //timer.setInitialTime(atoi(month), atoi(day), atoi(year), atoi(weekday), atoi(hour), atoi(minute), atoi(second), atoi(ms));
      timer.isTimeSet = true;
      timer.displayDateTime();
      RFduinoBLE.send('>');
    } else {
      RFduinoBLE.send(1);
      transferFlag = true;
    }
  } else {
    RFduinoBLE.send(0);
    transferFlag = false;
  }
}

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
