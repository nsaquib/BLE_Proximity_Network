/*
 * SensorsV9.ino - Network protocol for PrNet nodes.
 * Created by Dwyane George, January 20, 2015.
 * Social Computing Group, MIT Media Lab
 */

#include <PrNetRomManager.h>
#include <SimbleeCOM.h>
#include <Time.h>

#define TX_POWER_LEVEL -8
#define PAGES_TO_TRANSFER 50
#define BAUD_RATE 9600
// Configuration Parameters
#define NETWORK_SIZE 3
#define START_HOUR 22
#define START_MINUTE 1
#define END_HOUR 23
#define END_MINUTE 59
#define PACKET_DELAY 1000
#define SLEEP_DELAY 1000          // Assumed to be K * PACKET_DELAY for integer K
#define USE_SERIAL_MONITOR false

// Unique device ID
int deviceID;
// Data to transmit
char payload[1];
// Device BLE name
char deviceBLEName[2];
// RSSI total and count for each device
int rssiTotal[NETWORK_SIZE];
int rssiCount[NETWORK_SIZE];
// Boolean on if collecting samples
bool collectSamples = false;
// Time data structure
Time timer;
// ROM Manager for writing to flash ROM
PrNetRomManager writeROMManager;
// Transfer data flag to cell phone app
bool transferFlag = false;
// Counter for current ROM page
int pageCounter = STORAGE_FLASH_PAGE;
// Counter for current ROM page row
int rowCounter = 0;
// Boolean on use of SimbleeCOM stack
bool broadcasting = false;
// Counter for time in milliseconds
unsigned long packetStartTime = 0;
// Device ESN Matrix
const unsigned int ESNs[3][20] = {
  // Wildflower ESNs
  { 1, 1, 1, 1, 1 },
  // Aster ESNs
  { 1, 1, 1, 1, 1 },
  // Simblee DIP ESNs
  { 4173946190, 783932000, 2763040268 }
};

/*
 * Erases ROM memory and sets radio parameters
 */
void setup() {
  enableSerialMonitor();
  getDeviceID();
  deviceBLEName[0] = (deviceID < 10) ? deviceID + '0' : ((deviceID - (deviceID % 10)) / 10) + '0';
  deviceBLEName[1] = (deviceID < 10) ? 0 : (deviceID % 10) + '0';
  waitForParameters();
//  timer.isTimeSet = true;
//  timer.initialTime.day = 1;
  eraseROM();
  SimbleeCOM.txPowerLevel = TX_POWER_LEVEL;
  SimbleeCOM.mode = LOW_LATENCY;
  struct currentTime t = timer.getTime();
  delay(max(0, PACKET_DELAY - ((t.seconds * 1000 + t.ms) % PACKET_DELAY)));
}

/*
 * If in data collection period, collect data, otherise sleep device
 */
void loop() {
  if (timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE)) {
    collectData();
  } else {
    sleepUntilStartTime();
  }
}

/*
 * Polls host device for DEVICE_LOOPS times with DEVICE_LOOP_TIME delay between polls
 */
void collectData() {
  startBroadcast();
  if (timer.isTime(&packetStartTime, PACKET_DELAY)) {
    updateROMTable();
    timer.displayDateTime();
    pauseDataCollection();
  }
}

/*
 * Sleeps device between data collection periods
 */
void pauseDataCollection() {
  if (SLEEP_DELAY > 0) {
    stopBroadcast();
    struct currentTime t = timer.getTime();
    packetStartTime += max(0, SLEEP_DELAY - ((t.seconds * 1000 + t.ms) % PACKET_DELAY));
    Simblee_ULPDelay(max(0, SLEEP_DELAY - ((t.seconds * 1000 + t.ms) % PACKET_DELAY)));
  }
}

/*
 * Maps ESN to unique device ID less than NETWORK_SIZE
 */
void getDeviceID() {
  unsigned int esn = SimbleeCOM.getESN();
  Serial.println("ESN: " + String(esn));
  for (int school = 0; school < sizeof(ESNs) / sizeof(unsigned int); school++) {
    for (int id = 0; id < sizeof(ESNs[school]) / sizeof(unsigned int); id++) {
      if (esn == ESNs[school][id]) {
        deviceID = id;
        payload[0] = deviceID;
        Serial.println("Device ID: " + String(deviceID));
        return;
      }
    }
  }
}

////////// Callback Functions //////////

/*
 * Collects RSSI values and responds to incoming data transmissions
 */
void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi) {
  if ((int) payload[0] >= 0 && (int) payload[0] < NETWORK_SIZE) {
    rssiTotal[(int) payload[0]] += rssi;
    rssiCount[(int) payload[0]]++;
  }
}

////////// Time Functions //////////

/*
 * Delays program executation until parameters are set through the serial monitor
 */
void waitForParameters() {
  Serial.begin(BAUD_RATE);
  Serial.println("Waiting for parameters...");
  while (Serial.available() == 0) {
    delay(5);
  }
  timer.initialMillis = millis();
  int month = Serial.parseInt();
  int date = Serial.parseInt();
  int year = Serial.parseInt();
  int day = Serial.parseInt();
  int hour = Serial.parseInt();
  int minute = Serial.parseInt();
  int second = Serial.parseInt();
  int ms = Serial.parseInt();
  timer.setInitialTime(month, date, year, day, hour, minute, second, ms);
  timer.displayDateTime();
  Serial.println("Received parameters.");
  if (!USE_SERIAL_MONITOR) {
    Serial.end();
  }
}

////////// Sleep Functions //////////

/*
 * Sleeps device until START_HOUR:START_MINUTE plus offset to shift each device's host time
 */
void sleepUntilStartTime() {
  stopBroadcast();
  writeTimeROMRow();
  if (rowCounter >= MAX_ROWS) {
    writePage();
  } else {
    writePartialPage(true);
  }
  struct sleepTime sleepTime = timer.getTimeUntilStartTime(START_HOUR, START_MINUTE);
  int sleepTimeMillis = sleepTime.ms + (1000 * sleepTime.seconds) + (60000 * sleepTime.minutes) + (3600000 * sleepTime.hours) + (86400000 * sleepTime.days);
  Serial.println("Sleeping for " + String(sleepTime.days) + ":" + String(sleepTime.hours) + ":" + String(sleepTime.minutes) + ":" + String(sleepTime.seconds) + ":" + String(sleepTime.ms));
  disableSerialMonitor();
  Simblee_ULPDelay(sleepTimeMillis);
  writeTimeROMRow();
  enableSerialMonitor();
}

////////// Radio Functions //////////

/*
 * Enables radio broadcasting
 */
void startBroadcast() {
  if (!broadcasting) {
    SimbleeCOM.begin();
    SimbleeCOM.send(payload, sizeof(payload));
    broadcasting = true;
  }
}

/*
 * Disables radio broadcasting
 */
void stopBroadcast() {
  if (broadcasting) {
    SimbleeCOM.end();
    broadcasting = false;
  }
}

////////// Serial Monitor Functions //////////

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

////////// ROM Functions //////////

/*
 * Stores deviceID (I), RSSI (R), and time (T) as I,IRR,TTT,TTT with maximum value of 4,294,967,295
 */
void updateROMTable() {
  for (int i = 0; i < NETWORK_SIZE; i++) {
    int rssiAverage = (rssiCount[i] == 0) ? -128 : rssiTotal[i] / rssiCount[i];
    Serial.println(String(i) + "\t" + String(rssiAverage) + "\t" + String(rssiCount[i]));
    if (rssiAverage > -100) {
      if (rowCounter >= MAX_ROWS) {
        writePage();
      }
      struct currentTime t = timer.getTime();
      int data = t.seconds % 60;                        // Seconds
      data += (t.minutes % 60) * 100;                   // Minutes
      data += (t.hours % 24) * 10000;                   // Hours
      data += abs(rssiAverage % 100) * 1000000;         // RSSI
      data += (i % 42) * 100000000;                     // Device ID
      writeROMManager.table.data[rowCounter] = data;
      rowCounter++;
    }
    rssiTotal[i] = 0;
    rssiCount[i] = 0;
  }
  writePartialPage(false);
}

/*
 * Write a timestamp row HH:MM:SS to the ROM table
 */
void writeTimeROMRow() {
  if (rowCounter >= MAX_ROWS) {
    writePage();
  }
  struct currentTime t = timer.getTime();
  int data = t.seconds % 60;                            // Seconds
  data += (t.minutes % 60) * 100;                       // Minutes
  data += (t.hours % 24) * 10000;                       // Hours
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
  for (int i = STORAGE_FLASH_PAGE; i > STORAGE_FLASH_PAGE - 128; i--) {
    writeROMManager.erasePage(i);
  }
  Serial.println("Erased ROM");
}
