/*
 * SensorsV8.ino - Network protocol for PrNet nodes.
 * Created by Dwyane George, January 14, 2015.
 * Social Computing Group, MIT Media Lab
 */

#include <PrNetRomManager.h>
#include <SimbleeBLE.h>
#include <SimbleeForMobile.h>
#include <Time.h>

#define TX_POWER_LEVEL 4
#define BLE_AD_INTERVAL 2000
#define PAGES_TO_TRANSFER 50
#define BAUD_RATE 9600
// Configuration Parameters
#define NETWORK_SIZE 3
#define START_HOUR 0
#define START_MINUTE 0
#define END_HOUR 23
#define END_MINUTE 0
#define PACKETS 20
#define PACKET_DELAY 50
#define SLEEP_DELAY 0
#define USE_SERIAL_MONITOR true


// The ID of the button which displays screen 2
int toScreen2ButtonID;

// The ID of the button which displays screen 1
int toScreen1ButtonID;

// The ID of the current screen being displayed
int currentScreen;
uint8_t textfield;

// Unique device ID
int deviceID;
// Data to transmit
char payload[1];
// Device BLE name
char deviceBLEName[4];
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
  deviceBLEName[0] = '#';
  deviceBLEName[1] = (deviceID < 10) ? deviceID + '0' : ((deviceID - (deviceID % 10)) / 10) + '0';
  deviceBLEName[2] = (deviceID < 10) ? 0 : (deviceID % 10) + '0';
  if (!timer.isTimeSet) {
    waitForParameters();
    eraseROM();
  }
  SimbleeCOM.txPowerLevel = TX_POWER_LEVEL;
  SimbleeCOM.begin();
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
 * Devices device until time is set
 */
void waitForParameters() {
  Serial.println("Waiting for parameters...");
  SimbleeCOM.end();
  //disableSerialMonitor();
  SimbleeForMobile.advertisementInterval = BLE_AD_INTERVAL;
  SimbleeForMobile.deviceName = deviceBLEName;
  SimbleeForMobile.domain = "twoscreens.simblee.com";
  SimbleeForMobile.begin();
  while (!timer.isTimeSet) {
    SimbleeForMobile.process(); 
    delay(5);
    if (transferFlag) {
      //startTransfer();
    }
  }
  SimbleeForMobile.end();
  SimbleeCOM.begin();
  //enableSerialMonitor();
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
  //disableSerialMonitor();
  Simblee_ULPDelay(sleepTimeMillis);
  writeTimeROMRow();
  enableSerialMonitor();
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

////////// App Integration Functions //////////
void SimbleeForMobile_onConnect()
{
  currentScreen = -1;
}

void ui()
{  
  if(SimbleeForMobile.screen == currentScreen) return;
  
  currentScreen = SimbleeForMobile.screen;
  switch(SimbleeForMobile.screen)
  {
    case 1:
      createScreen1();
      break;
       
    case 2:
      createScreen2();
      break;
            
   default:
      Serial.print("ui: Uknown screen requested: ");
      Serial.println(SimbleeForMobile.screen);
  }
}

void ui_event(event_t &event)
{
  
  if(event.id == toScreen1ButtonID && event.type == EVENT_RELEASE && currentScreen == 2)
  {
    SimbleeForMobile.showScreen(1);
  } else if(event.id == textfield && currentScreen == 1) 
  { 
    Serial.println(event.text);
    timer.initialMillis = millis();
    timer.setInitialTime(
        (event.text[0] - '0') * 10 + (event.text[1] - '0'),                             // Month
        (event.text[2] - '0') * 10 + (event.text[3] - '0'),                             // Date
        (event.text[4] - '0') * 10 + (event.text[5] - '0'),                             // Year
        (event.text[6] - '0'),                                                    // Day
        (event.text[7] - '0') * 10 + (event.text[8] - '0'),                             // Hour
        (event.text[9] - '0') * 10 + (event.text[10] - '0'),                           // Minute
        (event.text[11] - '0') * 10 + (event.text[12] - '0'),                           // Second
        (event.text[13] - '0') * 100 + (event.text[14] - '0') * 10 + (event.text[15] - '0')); // Millisecond
    timer.displayDateTime();
    //writeTimeROMRow();
    SimbleeForMobile.showScreen(2);
  }
}

void createScreen1()
{
  SimbleeForMobile.beginScreen(WHITE);

  int textID = SimbleeForMobile.drawText(10, 60, "Simblee App", BLACK, 30);
  int textID2 = SimbleeForMobile.drawText(10, 120, "Enter the time.", BLACK, 20);
  int textID3 = SimbleeForMobile.drawText(10, 200, "When you touch out of the keyboard,", BLACK, 15);
  int textID4 = SimbleeForMobile.drawText(10, 225, "the time will be submitted.", BLACK, 15);
  textfield = SimbleeForMobile.drawTextField(10, 325, 250, "MMddyyEHHmmssSSS");
  SimbleeForMobile.setEvents(toScreen2ButtonID, EVENT_RELEASE);
  SimbleeForMobile.endScreen();
}

void createScreen2()
{
  SimbleeForMobile.beginScreen(WHITE);

  int textID = SimbleeForMobile.drawText(0, 60, "Simblee App", BLACK, 40);
  toScreen1ButtonID = SimbleeForMobile.drawButton(100, 200, 100, "Go Back");

  SimbleeForMobile.setEvents(toScreen1ButtonID, EVENT_RELEASE);
  SimbleeForMobile.endScreen();
}
