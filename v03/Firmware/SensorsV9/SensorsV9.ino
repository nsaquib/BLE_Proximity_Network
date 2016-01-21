/*
 * SensorsV9.ino - Network protocol for PrNet nodes.
 * Created by Dwyane George, January 20, 2015.
 * Social Computing Group, MIT Media Lab
 */

#include <PrNetRomManager.h>
#include <SimbleeCOM.h>
#include <Time.h>

#define TX_POWER_LEVEL 4
#define AD_INTERVAL 2000
#define PAGES_TO_TRANSFER 50
#define BAUD_RATE 9600
// Configuration Parameters
#define NETWORK_SIZE 3
#define START_HOUR 17
#define START_MINUTE 20
#define END_HOUR 23
#define END_MINUTE 0
#define PACKETS 10
#define PACKET_DELAY 100
#define SLEEP_DELAY 0
#define USE_SERIAL_MONITOR true

// Unique device ID
int deviceID;
// Data to transmit
char payload[1];
// Device BLE name
char deviceBLEName[2];
// RSSI total and count for each device
int rssiTotal[NETWORK_SIZE];
int rssiCount[NETWORK_SIZE];
// Time data structure
Time timer;
// ROM Manager for writing to flash ROM
PrNetRomManager writeROMManager;
// Transfer data flag to cell phone app
boolean transferFlag = false;
// Counter for current ROM page
int pageCounter = STORAGE_FLASH_PAGE;
// Counter for current ROM page row
int rowCounter = 0;
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
  eraseROM();
  SimbleeCOM.txPowerLevel = TX_POWER_LEVEL;
  SimbleeCOM.begin();
  struct currentTime t = timer.getTime();
  delay(max(0, (PACKETS * PACKET_DELAY) - ((t.seconds * 1000 + t.ms) % (PACKETS * PACKET_DELAY))));
}


/*
 * If in data collection period, collect data, otherise sleep device
 */
void loop() {
  if (timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE)) {
    collectData();
    pauseDataCollection();
  } else {
    sleepUntilStartTime();
  }
}

/*
 * Polls host device for DEVICE_LOOPS times with DEVICE_LOOP_TIME delay between polls
 */
void collectData() {
  timer.displayDateTime();
  for (int i = 0; i < PACKETS; i++) {
    if (!timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE)) {
      return;
    }
    SimbleeCOM.send(payload, sizeof(payload));
    struct currentTime t = timer.getTime();
    delay(max(0, PACKET_DELAY - ((t.seconds * 1000 + t.ms) % PACKET_DELAY)));
  }
  updateROMTable();
}

/*
 * Sleeps device between data collection periods
 */
void pauseDataCollection() {
  if (SLEEP_DELAY > 0) {
    SimbleeCOM.end();
    struct currentTime t = timer.getTime();
    Simblee_ULPDelay(max(0, SLEEP_DELAY - ((t.seconds * 1000 + t.ms) % PACKET_DELAY)));
    SimbleeCOM.begin();
  }
}

/*
 * Maps ESN to unique device ID less than NETWORK_SIZE
 */
void getDeviceID() {
  unsigned int esn = SimbleeCOM.getESN();
  Serial.print("ESN: ");
  Serial.println(esn);
  for (int school = 0; school < sizeof(ESNs)/sizeof(unsigned int); school++) {
    for (int id = 0; id < sizeof(ESNs[school])/sizeof(unsigned int); id++) {
      if (esn == ESNs[school][id]) {
        deviceID = id;
        payload[0] = deviceID;
        Serial.print("Device ID: ");
        Serial.println(deviceID);
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
  SimbleeCOM.end();
  writeTimeROMRow();
  if (rowCounter >= MAX_ROWS) {
    writePage();
  } else {
    writePartialPage(true);
  }
  struct sleepTime sleepTime = timer.getTimeUntilStartTime(START_HOUR, START_MINUTE);
  int sleepTimeMillis = sleepTime.ms + (1000 * sleepTime.seconds) + (60000 * sleepTime.minutes) + (3600000 * sleepTime.hours) + (86400000 * sleepTime.days);
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
  disableSerialMonitor();
  Simblee_ULPDelay(sleepTimeMillis);
  writeTimeROMRow();
  enableSerialMonitor();
  SimbleeCOM.begin();
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
    Serial.print(i);
    Serial.print(",");
    Serial.print(rssiAverage);
    Serial.print(",");
    Serial.println(rssiCount[i]);
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
  for (int i = STORAGE_FLASH_PAGE; i > STORAGE_FLASH_PAGE - 180; i--) {
    writeROMManager.erasePage(i);
  }
  Serial.println("Erased ROM");
}
