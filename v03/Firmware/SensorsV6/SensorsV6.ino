/*
 * SensorsV6.ino - Network protocol for PrNet nodes.
 * Created by Dwyane George, December 24, 2015.
 * Social Computing Group, MIT Media Lab
 */

#include <PrNetRomManager.h>
#include <RFduinoBLE.h>
#include <RFduinoGZLL.h>
#include <Time.h>

#define TX_POWER_LEVEL -4
#define BLE_AD_INTERVAL 2000
#define PAGES_TO_TRANSFER 50
#define HBA 0x000
#define BAUD_RATE 9600
// Configuration Parameters
#define MAX_DEVICES 2
#define START_HOUR 9
#define START_MINUTE 0
#define END_HOUR 23
#define END_MINUTE 0
#define HOST_LOOP_TIME 1000
#define HOST_LOOPS 1
#define DEVICE_LOOP_TIME 100
#define REGION_TRACKER false
#define REGION_TRACKER_DELAY 0
#define USE_SERIAL_MONITOR true

// Unique device ID
const int deviceID = 0;
// Device BLE name
char deviceBLEName[2];
// Device loops
const int DEVICE_LOOPS = (REGION_TRACKER) ? 0 : (HOST_LOOP_TIME*HOST_LOOPS)*(MAX_DEVICES-1)/(DEVICE_LOOP_TIME);
// Initial device role
device_t deviceRole = HOST;
// RSSI total and count for each device
int rssiTotal[MAX_DEVICES];
int rssiCount[MAX_DEVICES];
// Collect samples flag
boolean collectSamples = false;
// Time data structure
Time timer;
// ROM Manager for writing to flash ROM
PrNetRomManager writeROMManager;
// Transfer data flag to cell phone app
boolean transferFlag = false;
// Counter for current write page
int pageCounter = STORAGE_FLASH_PAGE;
// Counter for current ROM page row
int rowCounter = 0;

/*
 * Erases ROM memory and sets radio parameters
 */
void setup() {
  enableSerialMonitor();
  deviceBLEName[0] = (deviceID < 10) ? deviceID + '0' : ((deviceID - (deviceID % 10)) / 10) + '0';
  deviceBLEName[1] = (deviceID < 10) ? 0 : (deviceID % 10) + '0';
  RFduinoGZLL.txPowerLevel = TX_POWER_LEVEL;
  RFduinoGZLL.hostBaseAddress = HBA;
  RFduinoBLE.deviceName = deviceBLEName;
  if (!timer.isTimeSet) {
    waitForParameters();
    eraseROM();
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
  if (!timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE)) {
    sleepUntilStartTime();
    setupHost();
  } else {
    if (deviceRole == HOST) {
      loopHost();
    } else {
      loopDevice();
    }
  }
}

/*
 * Collects data samples HOST_LOOPS time for HOST_LOOP_TIME milliseconds
 */
void loopHost() {
  Serial.println("HOST");
  timer.displayDateTime();
  for (int i = 0; i < HOST_LOOPS; i++) {
    if (!timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE)) {
      return;
    }
    collectSamplesFromDevices();
    updateROMTable();
  }
  transitionHost();
}

/*
 * Polls host device for DEVICE_LOOPS times with DEVICE_LOOP_TIME delay between polls
 */
void loopDevice() {
  Serial.println("DEVICE");
  timer.displayDateTime();
  for (int i = 0; i < DEVICE_LOOPS; i++) {
    if (!timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE)) {
      return;
    }
    delayDevice();
    RFduinoGZLL.sendToHost(deviceID);
    if ((i + 1) % (HOST_LOOP_TIME / DEVICE_LOOP_TIME) == 0) {
      updateROMTable();
    }
  }
  setupHost();
}

////////// Host Functions //////////

/*
 * Turns on data collection flag for HOST_LOOP_TIME milliseconds
 */
void collectSamplesFromDevices() {
  collectSamples = true;
  timer.updateTime();
  timer.delayTime(HOST_LOOP_TIME - ((timer.currentTime.seconds * 1000 + timer.currentTime.ms) % HOST_LOOP_TIME));
  collectSamples = false;
}

/*
 * Transitions host for next collection cycle
 */
void transitionHost() {
  if (!REGION_TRACKER) {
    return setupDevice();
  } else {
    RFduinoGZLL.end();
    disableSerialMonitor();
    timer.updateTime();
    RFduino_ULPDelay(max(0, REGION_TRACKER_DELAY - ((timer.currentTime.seconds * 1000 + timer.currentTime.ms) % HOST_LOOP_TIME)));
    enableSerialMonitor();
    RFduinoGZLL.begin(deviceRole);
  }
}

////////// Device Functions //////////

/*
 * Delays device in ultra low power state for DEVICE_LOOP_TIME
 */
void delayDevice() {
  timer.delayTime(10);
  timer.updateTime();
  RFduino_ULPDelay(DEVICE_LOOP_TIME - ((timer.currentTime.seconds * 1000 + timer.currentTime.ms) % DEVICE_LOOP_TIME));
}

////////// Callback Functions //////////

/*
 * Collects RSSI values and responds to incoming data transmissions
 */
void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
  if (deviceRole == HOST && collectSamples && (int) data[0] >= 0 && (int) data[0] < MAX_DEVICES) {
    rssiTotal[(int) data[0]] += rssi;
    rssiCount[(int) data[0]]++;
    RFduinoGZLL.sendToDevice(device, deviceID);
  }
  if (deviceRole != HOST && (int) data[0] >= 0 && (int) data[0] < MAX_DEVICES) {
    rssiTotal[(int) data[0]] += rssi;
    rssiCount[(int) data[0]]++;
  }
}

////////// Time Functions //////////

/*
 * Devices device until time is set
 */
void waitForParameters() {
  Serial.println("Waiting for parameters...");
  disableSerialMonitor();
  RFduinoBLE.advertisementInterval = BLE_AD_INTERVAL;
  RFduinoBLE.begin();
  while (!timer.isTimeSet) {
    timer.delayTime(5);
    if (transferFlag) {
      startTransfer();
    }
  }
  RFduinoBLE.end();
  enableSerialMonitor();
}

////////// Sleep Functions //////////

/*
 * Sleeps device until START_HOUR:START_MINUTE plus offset to shift each device's host time
 */
void sleepUntilStartTime() {
  RFduinoGZLL.end();
  writeTimeROMRow();
  if (rowCounter >= MAX_ROWS) {
    writePage();
  } else {
    writePartialPage(true);
  }
  struct sleepTime sleepTime = timer.getTimeUntilStartTime(START_HOUR, START_MINUTE);
  int sleepTimeMillis = sleepTime.ms + (1000 * sleepTime.seconds) + (60000 * sleepTime.minutes) + (3600000 * sleepTime.hours) + (86400000 * sleepTime.days);
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
  disableSerialMonitor();
  RFduino_ULPDelay(sleepTimeMillis);
  writeTimeROMRow();
  enableSerialMonitor();
}

////////// Serial Monitor Code //////////

/*  
 * Enables the serial monitor
 */
void enableSerialMonitor() {
  if (USE_SERIAL_MONITOR) {
    Serial.begin(BAUD_RATE);
  }
}

/*  
 * Disables the serial monitor
 */
void disableSerialMonitor() {
  if (USE_SERIAL_MONITOR) {
    Serial.end();
  }
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
      timer.updateTime();
      int data = timer.currentTime.seconds % 60;        // Seconds
      data += (timer.currentTime.minutes % 60) * 100;   // Minutes
      data += (timer.currentTime.hours % 24) * 10000;   // Hours
      data += abs(rssiAverage % 100) * 1000000;         // RSSI
      data += (i % 42) * 100000000;                     // Device ID
      writeROMManager.table.data[rowCounter] = data;
      rowCounter++;
    }
    rssiTotal[i] = 0;
    rssiCount[i] = 0;
  }
  if (deviceRole == HOST) {
    writePartialPage(false);
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
  int data = timer.currentTime.seconds % 60;        // Seconds
  data += (timer.currentTime.minutes % 60) * 100;   // Minutes
  data += (timer.currentTime.hours % 24) * 10000;   // Hours
  writeROMManager.table.data[rowCounter] = data;
  rowCounter++;
}

/*
 * Persists the table to nonvolatile ROM memory
 */
void writePage() {
  int success = writeROMManager.writePage(STORAGE_FLASH_PAGE - writeROMManager.pageCounter, writeROMManager.table);
  Serial.println(success);
  rowCounter = 0;
}

/*
 * Persists a partially filled table to nonvolatile ROM memory
 */
void writePartialPage(bool advanceRowCounter) {
  for (int i = rowCounter; i < MAX_ROWS; i++) {
    writeROMManager.table.data[i] = -1;
  }
  int success = writeROMManager.writePartialPage(STORAGE_FLASH_PAGE - writeROMManager.pageCounter, writeROMManager.table);
  Serial.println(success);
  if (advanceRowCounter) {
    rowCounter++;
  }
}

/*
 * Erases all existing ROM data
 */
void eraseROM() {
  for (int i = STORAGE_FLASH_PAGE; i > STORAGE_FLASH_PAGE - 180; i--) {
    writeROMManager.erasePage(i);
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
  PrNetRomManager readROMManager;
  while (transferFlag) {
    readROMManager.loadPage(pageCounter);
    Serial.print("Sending page ");
    Serial.println(pageCounter);
    // Generate the next packet
    for (int i = 0; i < MAX_ROWS; i++) {
      char dataBuffer[10];
      sprintf(dataBuffer, "%d", readROMManager.table.data[i]);
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
