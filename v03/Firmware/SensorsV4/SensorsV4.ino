/*
 * Dwyane George
 * Social Computing Group, MIT Media Lab
 * All rights reserved.
 */

// Maximum devices in network
#define MAX_DEVICES 2
#define MAX_ROWS 80 
// Time range to perform data collection
#define START_HOUR 9
#define START_MINUTE 0
#define END_HOUR 20
#define END_MINUTE 0
// Host time
#define HOST_LOOP_TIME 2500
#define HOST_LOOPS 1
// Device time
#define DEVICE_LOOP_TIME 250

#include <RFduinoGZLL.h>
#include <RFduinoBLE.h>
#include <PrNetRomManager.h>
#include <stdlib.h>

/*
 * Start State is HOST
 * On iteration: DEVICEX = deviceID % 8
 * ID DEVICEX
 *  0 DEVICE0
 *  1 DEVICE1
 *  2 DEVICE2
 *  ...
 */
const int deviceID = 0;

// Global timer
struct timer {
  int hours = 10;
  int minutes = 0;
  int seconds = 0;
  int ms = 0;
};

// Device loops
const int DEVICE_LOOPS = (DEVICE_LOOP_TIME == 0) ? 0 : floor(((HOST_LOOP_TIME*HOST_LOOPS)*(MAX_DEVICES-1)/(DEVICE_LOOP_TIME)));
// BLE advertisement device name
const String BLE_NAME = (String) deviceID;
// Timer
struct timer timer;
// Host base addresses, HBA cannot be 0x55 or 0xaa
const int HBA = 0x000;
// Initial role
device_t deviceRole = HOST;
// Device roles
const device_t deviceRoles[] = {DEVICE0, DEVICE1, DEVICE2, DEVICE3, DEVICE4, DEVICE5, DEVICE6, DEVICE7};
// Loop counters
int hostCounter = 0;
int deviceCounter = 0;
// RSSI total and count for each device for averaging
float rssiTotal[MAX_DEVICES];
float rssiCount[MAX_DEVICES];
float rssiAverages[MAX_DEVICES];
// Collect samples flag
int collectSamples = 0;
// Pin for the green LED
int greenLED = 3;
// Date variables
int MONTH = 11;
int DAY = 6;
int YEAR = 15;
int WEEKDAY = 5;
// ROM Managers
PrNetRomManager m;  //for writing to flash ROM
PrNetRomManager m2; // for reading flash ROM and sending through BLE
// Counter for current ROM page row
int rowCounter = 0;
// Flag to transfer data to cell phone app
boolean transferFlag = false;
// Counter for current page to write to
int pageCounter = STORAGE_FLASH_PAGE;
int stop_len = STORAGE_FLASH_PAGE - 49;
boolean ble_setup_flag = false;

// Flag used to start sending
//boolean flag = false;
//int start;
// Variables used in packet generation
//int ch;
//int packet;
// 1K page with 12 bytes rows and 80 rows = 960 bytes.
// so each 1K page has 960/20 = 48 packets.
//int packets = 48;

void setup() {
  pinMode(greenLED, OUTPUT);
  // Adjust power output levels
  RFduinoGZLL.txPowerLevel = 4;
  // Set BLE parameters
  RFduinoBLE.deviceName = "0";
  // Set host base address
  RFduinoGZLL.hostBaseAddress = HBA;
  // Start the serial monitor
  Serial.begin(9600);
  
  // Setup for specific device roles
  if (deviceRole == HOST) {
    setupHost();
  }
  else {
    setupDevice();
  }
}

void setupHost() {
  RFduinoGZLL.end();
  RFduinoGZLL.begin(deviceRole);
}

void setupDevice() {
  RFduinoGZLL.end();
  RFduinoGZLL.begin(deviceRole);
}

void loop() {
  // Time is not set
  if (!timeIsSet()) {
    Serial.println("Waiting for time from phone app...");
    setTimer();
  }
  // Time is set
  else {
    if (!inDataCollectionPeriod()) {
      sleepUntilStartTime();
      if (transferFlag) {
        startTransfer();
      }
    }
    else {
      // Loop for specific device roles
      if (deviceRole == HOST) {
        loopHost();
      }
      else {
        loopDevice();
      }
    }
  }
}

void loopHost() {
  Serial.println("My role is HOST");
  if (inDataCollectionPeriod()) {
    resetRSSI();
    collectSamplesFromDevices();
    calculateRSSIAverages();
    updateROMTable();
    if (shouldBeDevice()) {
      switchToDevice();
    }
  }
}

void loopDevice() {
  if (inDataCollectionPeriod()) {
    // Sleep device
    //sleepDevice(DEVICE_LOOP_TIME);
    timeDelay(DEVICE_LOOP_TIME);
    //sleepDevice(DEVICE_LOOP_TIME);
    // Send data to all other hosts
    pollHost();
    if (shouldBeHost()) {
      switchToHost();
    }
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
  timeDelay(HOST_LOOP_TIME);
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

////////// Device Functions //////////

void pollHost() {
  // Only a device can poll the host
  if (deviceRole != HOST) {
    // Send deviceID to host
    RFduinoGZLL.sendToHost(deviceID);
  }
}

////////// Role Switch Functions //////////

boolean shouldBeDevice() {
  if (hostCounter >= HOST_LOOPS - 1) {
    hostCounter = 0;
    return true;
  }
  else {
    hostCounter++;
    return false;
  }
}

boolean shouldBeHost() {
  if (deviceCounter >= DEVICE_LOOPS - 1) {
    deviceCounter = 0;
    return true;
  }
  else {
    deviceCounter++;
    return false;
  }
}

void switchToHost() {
  deviceRole = HOST;
  setupHost();
}

void switchToDevice() {
  deviceRole = assignDeviceT();
  Serial.print("My role is DEVICE");
  Serial.println(deviceRole);
  setupDevice();
}

device_t assignDeviceT() {
  int deviceNum = (int) (deviceID % 8);
  return deviceRoles[deviceNum];
}

////////// Callback Functions //////////

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
  // Ignore device if outside range, should never occur
  // If collecting samples, update the RSSI total and count
  if (deviceRole == HOST && collectSamples && (int) data[0] >= 0 && (int) data[0] < MAX_DEVICES) {
    rssiTotal[(int) data[0]] += rssi;
    rssiCount[(int) data[0]]++;
  }
}

////////// Time Functions //////////

void timeDelay(int ms) {
  delay(ms);
  updateTime(ms);
}

boolean timeIsSet() {
  return !(timer.hours == 0 && timer.minutes == 0 && timer.seconds == 0 && timer.ms == 0);
}

void setTimer() {
  RFduinoGZLL.end();
  RFduinoBLE.begin();
  // Set local time from phone app
  while (!timeIsSet()) {
    timeDelay(100);
    if (transferFlag) {
      startTransfer();
    }
  }
  RFduinoBLE.end();
}

void sleepDevice(int milliseconds) {
  RFduinoGZLL.end();
  RFduinoBLE.begin();
  RFduino_ULPDelay(milliseconds);
  updateTime(milliseconds);
  RFduinoBLE.end();
  RFduinoGZLL.begin(deviceRole);
}

boolean inDataCollectionPeriod() {
  // Collect data while within range
  if ((timer.hours > START_HOUR) && (timer.hours < END_HOUR)) {
    return true;
  }
  // Inclusive on START_MINUTE and exclusive on END_MINUTE
  else if (((timer.hours == START_HOUR) && (timer.minutes >= START_MINUTE))
            || ((timer.hours == END_HOUR) && (timer.minutes < END_MINUTE))) {
    return true;
  }
  // Otherwise, do not collect data
  else {
    return false;
  }
}

void displayClockTime() {
  displayDate();
  Serial.print(timer.hours);
  Serial.print(":");
  Serial.print(timer.minutes);
  Serial.print(":");
  Serial.print(timer.seconds);
  Serial.print(":");
  Serial.println(timer.ms);
}

void displayDate() {
  Serial.print(MONTH);
  Serial.print("-");
  Serial.print(DAY);
  Serial.print("-");
  Serial.print(YEAR);
  Serial.print(" ");
}

void updateTime(int ms) {
  // Update time only if its been set
  if (!(timer.hours == 0 && timer.minutes == 0 && timer.seconds == 0 && timer.ms == 0)) {
    int carryOver;
    // Update each time component and propagate carryOver linearly
    timer.ms += ms;
    carryOver = floor(timer.ms/1000);
    if (timer.ms > 999) {
      timer.ms = timer.ms % 1000;
    }
    timer.seconds += carryOver;
    carryOver = floor(timer.seconds/60);
    if (timer.seconds > 59) {
      timer.seconds = timer.seconds % 60;
    }
    timer.minutes += carryOver;
    carryOver = floor(timer.minutes/60);
    if (timer.minutes > 59) {
      timer.minutes = timer.minutes % 60;
    }
    timer.hours += carryOver;
    carryOver = floor(timer.hours/24);
    if (timer.hours > 23) {
      timer.hours = timer.hours % 24;
    }
    DAY += carryOver;
    carryOver = floor(DAY/30);
    if (DAY > 30) {
      DAY = DAY % 30;
    }
    MONTH += carryOver;
    carryOver = floor(MONTH/12);
    if (MONTH > 12) {
      MONTH = MONTH % 12;
    }
    YEAR += carryOver;
    if (YEAR > 99) {
      YEAR = YEAR % 99;
    }
  }
}

void sleepUntilStartTime() {
  // Number of ms, seconds, minutes, and hours to delay
  int hours;
  int minutes;
  int seconds;
  int ms;
  if (timer.hours == START_HOUR) {
    if (timer.minutes < START_MINUTE) {
      hours = 0;
    }
    else {
      hours = 23;
    }
  }
  else if (timer.hours < START_HOUR) {
    if (timer.minutes < START_MINUTE) {
       hours = START_HOUR - timer.hours;
    }
    else {
      hours = START_HOUR - timer.hours - 1;
    }
  }
  else {
    if (timer.minutes < START_MINUTE) {
      hours = 24 - timer.hours + START_HOUR;
    }
    else {
      hours = 24 - timer.hours + START_HOUR - 1; 
    }
  }
  if (timer.seconds <= 0) {
    minutes = (60 - timer.minutes + START_MINUTE) % 60;
  }
  else {
    minutes = (60 - timer.minutes + START_MINUTE - 1) % 60;
  }
  if (timer.ms <= 0) {
    seconds = (60 - timer.seconds) % 60;
  }
  else {
    seconds = (60 - timer.seconds - 1) % 60;
  }
  ms = (1000 - timer.ms) % 1000;
  // Calculate time to START_HOUR:START_MINUTE by converting higher order time units to ms
  int delayTime = ms + 1000*seconds + 60000*minutes + 3600000*hours;
  // Add additional offset for each device
  int offset = HOST_LOOP_TIME*HOST_LOOPS*deviceID;
  delayTime += offset;
  Serial.print("Sleeping for ");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":");
  Serial.print(seconds);
  Serial.print(":");
  Serial.println(ms);
  Serial.print("Offset: ");
  Serial.println(offset);
  //timeDelay(delayTime);
  sleepDevice(delayTime);
}

////////// ROM Code //////////

void updateROMTable() {
  // Update rows for rom table
  int i;
  for (i = 0; i < MAX_DEVICES; i++) {
    if (rssiAverages[i] != -128) {
      if (rowCounter >= MAX_ROWS) {
        writePage();
      }
      m.table.t[rowCounter] = millis();
      m.table.id[rowCounter] = i;
      m.table.rsval[rowCounter] = rssiAverages[i];
      rowCounter++;
    }
  }
}

void writePage() {
  // Erase any existing data in the current rom page
  m.erasePage(STORAGE_FLASH_PAGE - m.pagecounter);
  // Write to rom memory
  int success = m.writePage(STORAGE_FLASH_PAGE - m.pagecounter, m.table);
  Serial.println(success);
  rowCounter = 0;
}

////////// App Integration Code //////////

void RFduinoBLE_onReceive(char *data, int len) {
  // If the first byte exists
  if (data[0]) {
    if(data[0] == '>') {
      //Update start time
      char startHour[2];
      startHour[0] = data[1];
      startHour[1] = data[2];

      char startMin[2];
      startMin[0] = data[4];
      startMin[1] = data[5];

      char startSec[2];
      startSec[0] = data[7];
      startSec[1] = data[8];
      
      char startMilliSec[2];
      startMilliSec[0] = data[10];
      startMilliSec[1] = data[11];
      
      timer.hours = atoi(startHour);
      timer.minutes = atoi(startMin);
      timer.seconds = atoi(startSec);
      timer.ms = atoi(startMilliSec);
      Serial.print("Hour: ");
      Serial.println(timer.hours);
      Serial.print("Minute: ");
      Serial.println(timer.minutes);
      Serial.print("Second: ");
      Serial.println(timer.seconds);
      Serial.print("Millisecond:");
      Serial.println(timer.ms);
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
    m2.loadPage(pageCounter);
    Serial.print("Starting page: ");
    Serial.println(pageCounter);
    
    // Generate the next packet
    for (int i = 0; i < lenrec; i++) {
      Serial.println(m2.table.t[i]);
      char space = ' ';
      char buf_t[10];
      char buf_id[10];
      char buf_rsval[10];
      sprintf(buf_t, "%d", m2.table.t[i]);
      sprintf(buf_id, "%d", m2.table.id[i]);
      sprintf(buf_rsval, "%d", m2.table.rsval[i]);
      while (!RFduinoBLE.send(buf_t, 10));
      timeDelay(5); 
      while (!RFduinoBLE.send(buf_id, 10));
      timeDelay(5);
      while (!RFduinoBLE.send(buf_rsval, 10));
      while (!RFduinoBLE.send('|'));
    } 
    Serial.println("Finished with loop");
    while (!RFduinoBLE.send('#'));
    timeDelay(200);
    pageCounter--;
    if (pageCounter < stop_len) {
      Serial.println("Ending");
      transferFlag = false;
      RFduinoBLE.send(0);
    }
  }
}
