/*
 * Dwyane George
 * Social Computing Group, MIT Media Lab
 * All rights reserved.
 */

// Maximum devices in network
#define MAX_DEVICES 2
#define MAX_ROWS 80
// Time range to perform data collection
#define START_HOUR 16
#define START_MINUTE 30
#define END_HOUR 23
#define END_MINUTE 0
// Host time
#define HOST_LOOP_TIME 5000
#define HOST_LOOPS 1
// Device time
#define DEVICE_LOOP_TIME 200
#define lenrec 80

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

// Serialized time from Python script
struct timer {
  int hours = 0;
  int minutes = 0;
  int seconds = 0;
  int ms = 0;
};

// Device loops
const int DEVICE_LOOPS = floor(((HOST_LOOP_TIME*HOST_LOOPS)*(MAX_DEVICES-1)/(DEVICE_LOOP_TIME)));
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
int MONTH = 10;
int DAY = 26;
int YEAR = 15;
int WEEKDAY = 1;
// ROM Managers
PrNetRomManager m;  //for writing to flash ROM
PrNetRomManager m2; // for reading flash ROM and sending through BLE
int page_counter = STORAGE_FLASH_PAGE;
int stop_len = STORAGE_FLASH_PAGE - 49;

bool ble_setup_flag = false;
long loopCounter = 0;
long rowcounter = 0;

// send to phone vars
// flag used to start sending
bool flag = false;
int start;

bool transfer_flag = false;

// Variables used in packet generation
int ch;
int packet;

// 1K page with 12 bytes rows and 80 rows = 960 bytes.
// so each 1K page has 960/20 = 48 packets.
int packets = 48;

void setup() {
  pinMode(greenLED, OUTPUT);
  // Adjust power output levels
  RFduinoGZLL.txPowerLevel = 0;
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
    // Setup for specific device roles
    if (deviceRole == HOST) {
      setupHost();
    }
    else {
      setupDevice();
    }
  }
  // Time is set
  else {
    if (!inDataCollectionPeriod()) {
      sleepUntilStartTime();
      if (transfer_flag) {
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
    if (loopCounter >= (MAX_ROWS / MAX_DEVICES)) {
      writePage();
    }
    // Start GZZL stack from wake cycle
    RFduinoGZLL.end();
    RFduinoGZLL.hostBaseAddress = HBA;
    RFduinoGZLL.begin(HOST);
    resetRSSI();
    collectSamplesFromDevices();
    calculateRSSIAverages();
    updateROMTable();
    loopCounter++;
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
    if (transfer_flag) {
      startTransfer();
    }
  }
  RFduinoBLE.end();
}

void sleepDevice(int milliseconds) {
  RFduinoGZLL.end();
  RFduinoBLE.begin();
  updateTime(milliseconds);
  RFduino_ULPDelay(milliseconds);
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
  timeDelay(delayTime);
  //sleepDevice(delayTime);
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

////////// App Integration Code //////////

void RFduinoBLE_onReceive(char *data, int len) {
  Serial.println(data);
  // if the first byte is 0x01 / on / true
  if (data[0])
  {
    //Serial.println(data[0]);
    if(data[0] == '>')
    {
      //UPDATE START TIME
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
      Serial.print("Start Hour: ");
      Serial.println(timer.hours);
      Serial.print("Start Minute: ");
      Serial.println(timer.minutes);
      Serial.print("Start Sec: ");
      Serial.println(timer.seconds);
      Serial.print("Start MilliSec:");
      Serial.println(timer.ms);
      RFduinoBLE.send('>');
    }
    else{
      RFduinoBLE.send(1);
      transfer_flag = true;
    }
  }
  else 
  {
    RFduinoBLE.send(0);
    transfer_flag = false;
  }
}

void startTransfer() {
  while (transfer_flag)
  {
    m2.loadPage(page_counter);
    Serial.print("Starting page: ");
    Serial.println(page_counter);
    
    // generate the next packet
    for (int j = 0; j < lenrec; j++)
    {
      char space = ' ';
      char buf_t[10];
      char buf_id[10];
      char buf_rsval[10];
      sprintf(buf_t, "%d", m2.table.t[j]);
      sprintf(buf_id, "%d", m2.table.id[j]);
      sprintf(buf_rsval, "%d", m2.table.rsval[j]);
      while (! RFduinoBLE.send(buf_t, 10));
      while (! RFduinoBLE.send(space));
      timeDelay(20); 
      while (! RFduinoBLE.send(buf_id, 10));
      while (! RFduinoBLE.send(space));
      timeDelay(20);
      while (! RFduinoBLE.send(buf_rsval, 10));
      while (! RFduinoBLE.send(space));
      while (! RFduinoBLE.send('|'));
    } 
    Serial.println("Finished with loop");
    while (! RFduinoBLE.send('#'));
    timeDelay(200);
    page_counter--;
    if (page_counter < stop_len)
    {
      Serial.println("Ending");
      transfer_flag = false;
      RFduinoBLE.send(0);
    }
  }
}

