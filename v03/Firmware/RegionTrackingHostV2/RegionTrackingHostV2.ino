/*
 * Region Tracking Host -- keep a few hosts around the desired area who will continually listen
 * for other beacons that people are carrying around.
 * 
 * Social Computing Group, MIT Media Lab
 */

#define MAX_DEVICES 8
#define MAX_ROWS 80
#define HOST_LOOP_TIME 10000
#define HOST_SLEEP_TIME 10000

#include <RFduinoBLE.h>
#include <RFduinoGZLL.h>
#include <PrNetRomManager.h>

// RSSI total and count for each device for averaging
float rssiTotal[MAX_DEVICES];
float rssiCount[MAX_DEVICES];
float rssiAverages[MAX_DEVICES];

// Collect samples flag
int collectSamples = 0;
long loopCounter = 0;
// Time elapsed
long timePassed = 0; // in seconds
// Designated time to run
const long timeToRun = 10800; // in seconds
// Host base address
const int HBA = 0x000;
// Pin for the green LED
int greenLED = 3;

PrNetRomManager m;

void setup() {
  pinMode(greenLED, OUTPUT);
  RFduinoGZLL.txPowerLevel = 4;
  RFduinoGZLL.hostBaseAddress = 0x000;
  RFduinoGZLL.begin(HOST);
  Serial.begin(9600);
}

void loop() {
  if (timePassed < timeToRun) {
    Serial.println("My role is HOST");
    if(loopCounter >= (MAX_ROWS / MAX_DEVICES)) {
      writePage();
    }
    resetRSSI();
    collectSamplesFromDevices();
    calculateRSSIAverages();
    updateROMTable();
    loopCounter++;
    sleepHost(HOST_SLEEP_TIME);
    timePassed += HOST_SLEEP_TIME/1000;
  } else {
    sleepForever();
  }
}

////////// Host Functions //////////

void calculateRSSIAverages() {
  // Calculate the RSSI averages for each device
  int i;
  for (i = 0; i < MAX_DEVICES; i++) {
    // No samples received, set to the lowest RSSI
    // Prevents division by zero
    if (rssiCount[i] == 0) {
      rssiAverages[i] = -128;
    }
    else {
      rssiAverages[i] = rssiTotal[i] / rssiCount[i];
    }
    Serial.print("DEVICE");
    Serial.print(i);
    Serial.print(" ");
    Serial.print(rssiAverages[i]);
    Serial.print(" ");
    Serial.println(rssiCount[i]);
  }
}

void collectSamplesFromDevices() {
  // Start collecting RSSI samples
  collectSamples = 1;
  // Wait for designate host time/HBA
  delay(HOST_LOOP_TIME);
  // Stop collecting RSSI samples
  collectSamples = 0;
}

void resetRSSI() {
  // Reset the RSSI averaging for each device
  int i;
  for (i = 0; i < MAX_DEVICES; i++) {
    rssiTotal[i] = 0;
    rssiCount[i] = 0;
  }
}

void sleepHost(int milliseconds) {
  // sleep for some time
    Serial.println("Entering sleep");
    RFduinoBLE.begin();
    RFduino_ULPDelay(milliseconds);
    RFduinoBLE.end();
}

void sleepForever() {
  // Sleep host indefinitely
  Serial.println("Sleeping forever...");
  RFduinoBLE.begin();
  RFduino_ULPDelay(INFINITE);
  RFduinoBLE.end();
}

////////// Callback Functions //////////

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
  // Ignore device if outside range, should never occur
  // If collecting samples, update the RSSI total and count
  if (collectSamples && (int) data[0] >= 0 && (int) data[0] < MAX_DEVICES) {
    rssiTotal[(int) data[0]] += rssi;
    rssiCount[(int) data[0]]++;
  }
}

////////// ROM Code //////////

void updateROMTable() {
  // Update rows for rom table
  int i;
  for (i = 0; i < MAX_DEVICES; i++) {
    m.table.t[i + loopCounter * MAX_DEVICES] = millis();
    m.table.id[i + loopCounter * MAX_DEVICES] = i;
    m.table.rsval[i + loopCounter * MAX_DEVICES] = rssiAverages[i];
  }
}

void writePage() {
  // Erase any existing data in the current rom page
  m.erasePage(STORAGE_FLASH_PAGE - m.pagecounter);
  // Write to rom memory
  int success = m.writePage(STORAGE_FLASH_PAGE - m.pagecounter, m.table);
  Serial.println(success);
  loopCounter = 0;
}
