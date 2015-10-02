/*
Dwyane George
Social Computing Group, MIT Media Lab
All rights reserved.
*/

// Maximum devices in network
#define MAX_DEVICES 16
// Time range to perform data collection
#define START_HOUR 9 // 8
#define START_MINUTE 0 //45
#define END_HOUR 23 //1
#define END_MINUTE 0 //0
// Time as host: 5 seconds/HBA, time as device: 150 seconds
#define HOST_TIME 20000
#define DEVICE_TIME 25000 // 150000
// Devices poll host every 2.5 seconds
#define DEVICE_POLL_TIME 2300
#define POST_HOST_DELAY 500
#define DEVICE_POLL_COUNT 10 //60
// Number of HBA groups
#define HBA_GROUPS 2


#include <RFduinoGZLL.h>
#include <RFduinoBLE.h>

// Serialized time from Python script
struct timer {
  int hours = 0;
  int minutes = 0;
  int seconds = 0;
  int ms = 0;
};

struct timer timer;
int DAY = 1;
int MONTH = 10;
int YEAR = 15;
int WEEKDAY = 4;

// Device ID: 0...16
const int deviceID = 1;
device_t deviceRole = HOST;

// Device roles, host base addresses, and device base addresses
// HBA cannot be 0x55 or 0xaa
const int hostBaseAddresses[] = {0x000, 0x001}; 
//int hostBaseAddresses[] = {0x000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015};
//int deviceBaseAddresses[] = {0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x109, 0x110, 0x111, 0x112, 0x113, 0x114, 0x115};
const device_t deviceRoles[] = {DEVICE0, DEVICE1, DEVICE2, DEVICE3, DEVICE4, DEVICE5, DEVICE6, DEVICE7};

// Index for hostBaseAddresses
int hostCounter = 0;

// RSSI total and count for each device for averaging
int rssi_total[MAX_DEVICES];
int rssi_count[MAX_DEVICES];

// Device with the maximum RSSI
char closest_device = 0;

// Collect samples flag
int collect_samples = 0;

// Pin for the green LED
int greenLED = 3;

void setup() {
  pinMode(greenLED, OUTPUT);
  // Adjust power output levels
  RFduinoGZLL.txPowerLevel = 4;
  // Advertise for 4 seconds in BLE mode
  RFduinoBLE.advertisementInterval = 4000;
  // Start the serial monitor
  Serial.begin(57600);
  
  // Setup for specific device roles
  if (deviceRole == HOST) {
    setupHost();
  }
  else {
    setupDevice();
  }
}

void setupHost() {
  // Start the GZLL stack
  RFduinoGZLL.end();
  RFduinoGZLL.hostBaseAddress = hostBaseAddresses[0];
  RFduinoGZLL.begin(HOST);
}

void setupDevice() {
  RFduinoGZLL.end();
  RFduinoGZLL.hostBaseAddress = hostBaseAddresses[(int) floor(deviceID/8)];
  RFduinoGZLL.begin(deviceRole);
}

void loop() {
  displayClockTime();
  //Serial.println(deviceRole);
  // Time is not set
  if (!timeIsSet()) {
    Serial.println("Setting time...");
    setTimer();
  }
  // Time is  set
  else {
    if (!inDataCollectionPeriod()) {
      sleepUntilStartTime();
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
  timeDelay(1000);
}

void loopHost() {
  Serial.println("Loop Host");
  if (inDataCollectionPeriod()) {
    // Start GZZL stack from wake cycle
    Serial.print("Listening on HBA");
    Serial.println(hostCounter);
    RFduinoGZLL.end();
    RFduinoGZLL.hostBaseAddress = hostBaseAddresses[hostCounter];
    RFduinoGZLL.begin(HOST);
    //Serial.print("My hostCounter is: ");
    //Serial.println(hostCounter);
    int i;
    // Reset the RSSI averaging for each device
    for (i = 0; i < MAX_DEVICES; i++) {
      rssi_total[i] = 0;
      rssi_count[i] = 0;
    }
    // Start collecting RSSI samples
    collect_samples = 1;
    // Wait for designate host time/HBA
    timeDelay(HOST_TIME/HBA_GROUPS);
    // Stop collecting RSSI samples
    collect_samples = 0;
    // Calculate the RSSI avarages for each device
    int average[MAX_DEVICES];
    for (i = 0; i < MAX_DEVICES; i++) {
      // No samples received, set to the lowest RSSI
      // Prevents division by zero
      if (rssi_count[i] == 0)
        average[i] = -128;
      else
        average[i] = rssi_total[i] / rssi_count[i];
    }
    //Serial.println(shouldBeDevice());
    if (shouldBeDevice()) {
      switchToDevice();
    }
    // Increment the hostCounter such that it cycles from 0...number of cycles - 1
    hostCounter = (hostCounter + 1) % HBA_GROUPS;
    // Find the device with the maximum RSSI value
    /*int closest = 0;
    for (i = 1; i < MAX_DEVICES; i++)
      if (average[i] > average[closest])
        closest = i;
    closest_device = closest;*/
  }
}

void loopDevice() {
  Serial.println("Loop Device");
  if (inDataCollectionPeriod()) {
    RFduinoGZLL.end();
    int i;
    for (i = 0; i < DEVICE_POLL_COUNT; i++) {
      //RFduinoBLE.begin();
      // Sleep for DEVICE_POLL_TIME
      //RFduino_ULPDelay(MILLISECONDS(DEVICE_POLL_TIME));
      //updateTime(DEVICE_POLL_TIME);
      //RFduinoBLE.end();
      Serial.print("Device Iteration is: ");
      Serial.println(i);
      // Send data to all other hosts
      pollHost(deviceRole, hostBaseAddresses[(int) floor(deviceID/8)]);
    }
    switchToHost();
  }
}

void pollHost(device_t drole, int hostAddr) {
  // Only a device can poll the host
  if (deviceRole != HOST) {
    // Host base address to broadcast to
    RFduinoGZLL.hostBaseAddress = hostAddr;
    RFduinoGZLL.begin(drole);
    // Send null packet to host
    RFduinoGZLL.sendToHost(NULL, 0);
    timeDelay(POST_HOST_DELAY);
    // End GZLL stack
    RFduinoGZLL.end();
  }
}

void timeDelay(int ms) {
  delay(ms);
  updateTime(ms);
}

void setTimer() {
  // Set local time from Serial Monitor
  while (Serial.available() == 0) {
    delay(500);
  }
  if (Serial.available() > 0) {
    timer.hours = Serial.parseInt();
    timer.minutes = Serial.parseInt();
    timer.seconds = Serial.parseInt();
    timer.ms = Serial.parseInt();
  }
}

boolean timeIsSet() {
  return !(timer.hours == 0 && timer.minutes == 0 && timer.seconds == 0 && timer.ms == 0);
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

boolean inDataCollectionPeriod() {
  // Collect data while within range
  if ((timer.hours > START_HOUR) && (timer.hours < END_HOUR)) {
    return true;
  }
  // Inclusive on START_MINUTE and exclusive on END_MINUTE
  else if (((timer.hours == START_HOUR) && (timer.minutes >= START_MINUTE)) || ((timer.hours == END_HOUR) && (timer.minutes < END_MINUTE))) {
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

void sleepUntilStartTime() {
  // Number of ms, seconds, minutes, and hours to delay
  int hours;
  int minutes;
  int seconds;
  int ms;
  if (timer.hours < START_HOUR) {
    hours = START_HOUR - timer.hours;
  }
  else if (timer.hours == START_HOUR) {
    if (timer.minutes <= START_MINUTE) {
      hours = 0;
    }
    else {
      hours = 23;
    }
  }
  else {
    hours = 24 - timer.hours + START_HOUR;
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
  int offset = 60000*deviceID;
  delayTime += offset;
  Serial.print("Sleeping for ");
  Serial.print(delayTime);
  Serial.println(" ms");
  timeDelay(delayTime);
}

boolean shouldBeDevice() {
  if (hostCounter == HBA_GROUPS - 1) {
    Serial.println("It's time to become a DEVICE...");
  }
  return hostCounter == HBA_GROUPS - 1;
}

void switchToHost() {
  Serial.println("Becoming a host now...");
  deviceRole = HOST;
  Serial.println("My role is HOST");
  setupHost();
}

void switchToDevice() {
  
  Serial.println("Becoming a device now...");
  deviceRole = assignDeviceT();
  Serial.print("My role is DEVICE");
  Serial.println(deviceRole);
  setupDevice();
}

device_t assignDeviceT() {
  int deviceNum = (int) floor(deviceID/2);
  return deviceRoles[deviceNum];
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
  //if (deviceRole == HOST) {
    // Ignore device if outside range, should never occur
  if (device > MAX_DEVICES)
    return;
  if (RFduinoGZLL.hostBaseAddress == hostBaseAddresses[0]) {
    Serial.print("ID ");
    Serial.print(deviceID);
    Serial.print("from DEVICE");
    Serial.print(device);
    Serial.print(" (HBA0) ");
    Serial.print("RSSI: ");
    Serial.println(rssi);
  }
  if (RFduinoGZLL.hostBaseAddress == hostBaseAddresses[1]) {
    Serial.print("ID ");
    Serial.print(deviceID);
    Serial.print("from DEVICE");
    Serial.print(device);
    Serial.print(" (HBA0) ");
    Serial.print("RSSI: ");
    Serial.println(rssi);
  }
  // If collecting samples, update the RSSI total and count
  if (collect_samples) {
    if (RFduinoGZLL.hostBaseAddress == hostBaseAddresses[0]) {
      rssi_total[device] += rssi;
      rssi_count[device]++;
    }
    if (RFduinoGZLL.hostBaseAddress == hostBaseAddresses[1]) {
      rssi_total[8 + device] += rssi;
      rssi_count[8 + device]++;
    }
  }
  //}
}
