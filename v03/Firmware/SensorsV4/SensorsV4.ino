/*
 * SensorsV4.ino - Network protocol for PrNet nodes.
 * Created by Dwyane George, November 2, 2015.
 * Social Computing Group, MIT Media Lab
 */

#define TX_POWER_LEVEL 4
#define BLE_AD_INTERVAL 5000
#define PAGES_TO_TRANSFER 50
// Configuration Parameters
#define MAX_DEVICES 8
#define START_HOUR 9
#define START_MINUTE 0
#define END_HOUR 13
#define END_MINUTE 0
#define HOST_LOOP_TIME 1000
#define HOST_LOOPS 1
#define DEVICE_LOOP_TIME 100

#include <PrNetRomManager.h>
#include <RFduinoBLE.h>
#include <RFduinoGZLL.h>
#include <stdlib.h>
#include <Time.h>

// Unique device ID
const int deviceID = 0;
// Device loops
const int DEVICE_LOOPS = (DEVICE_LOOP_TIME == 0) ? 0 : (HOST_LOOP_TIME*HOST_LOOPS)*(MAX_DEVICES-1)/(DEVICE_LOOP_TIME);
// Host base addresses, HBA cannot be 0x55 or 0xaa
const int HBA = 0x000;
// Device roles
const device_t deviceRoles[] = {DEVICE0, DEVICE1, DEVICE2, DEVICE3, DEVICE4, DEVICE5, DEVICE6, DEVICE7};
// Pin for the green LED
const int greenLED = 3;
// Initial role
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
  //char BLE_NAME[2];
  //deviceIDString.toCharArray(BLE_NAME, 2);
  RFduinoBLE.deviceName = "0"; //BLE_NAME;
  RFduinoGZLL.hostBaseAddress = HBA;
  Serial.begin(9600);
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
  deviceRole = deviceRoles[(int) (deviceID % 8)];
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
  if (timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE)) {
    for (int i = 0; i < HOST_LOOPS; i++) {
      collectSamplesFromDevices();
      updateROMTable();
    }
    setupDevice();
  }
}

void loopDevice() {
  if (timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE)) {
    for (int i = 0; i < DEVICE_LOOPS; i++) {
      //sleepDevice(DEVICE_LOOP_TIME);
      timer.delayTime(DEVICE_LOOP_TIME);
      pollHost();
    }
    setupHost();
  }
}

////////// Host Functions //////////

void collectSamplesFromDevices() {
  collectSamples = true;
  timer.delayTime(HOST_LOOP_TIME);
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
  RFduinoBLE.begin();
  Serial.println("Waiting for time...");
  // Wait for time
  while (!timer.isTimeSet) {
    timer.delayTime(10);
    if (transferFlag) {
      startTransfer();
    }
  }
  RFduinoBLE.end();
}

////////// Sleep Functions //////////

void sleepDevice(int milliseconds) {
  RFduinoGZLL.end();
  RFduinoBLE.advertisementInterval = BLE_AD_INTERVAL;
  RFduinoBLE.begin();
  RFduino_ULPDelay(milliseconds);
  timer.updateTime(milliseconds);
  RFduinoBLE.end();
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
    Serial.print(i);
    Serial.print(",");
    Serial.print(rssiAverage);
    Serial.print(",");
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
      char startHour[2];
      char startMinute[2];
      char startSecond[2];
      char startMS[2];
      startHour[0] = data[1];
      startHour[1] = data[2];
      startMinute[0] = data[4];
      startMinute[1] = data[5];
      startSecond[0] = data[7];
      startSecond[1] = data[8];
      startMS[0] = data[10];
      startMS[1] = data[11];
      // Set time
      timer.setTime(atoi(startHour), atoi(startMinute), atoi(startSecond), atoi(startMS));
      // TODO: get date from app and set date
      timer.setDate(0, 0, 0, 0);
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
