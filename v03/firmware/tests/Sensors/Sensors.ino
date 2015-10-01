/*
Dwyane George
Social Computing Group, MIT Media Lab
All rights reserved.
*/

// Maximum devices in network
#define MAX_DEVICES 16
// Time range to perform data collection
#define START_HOUR 9
#define START_MINUTE 0
#define END_HOUR 13
#define END_MINUTE 0

// Time as host: 1 second, time as device: 15 seconds
#define timeAsHost 1000
#define timeAsDevice 15000

#include <RFduinoGZLL.h>
#include <RFduinoBLE.h>

// Serialized time from Python script
struct timer {
  int hour = 0;
  int minute = 0;
  int second = 0;
  int ms = 0;
};

struct timer timer;
int DAY = 1;
int MONTH = 10;
int YEAR = 15;
int WEEKDAY = 4;
boolean timeSet = false;

// Device ID: 0...16
int deviceID = 0;
device_t deviceRole = HOST;

// Device roles, host base addresses, and device base addresses
int hostBaseAddresses[] = {0x000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015};
//int deviceBaseAddresses[] = {0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x109, 0x110, 0x111, 0x112, 0x113, 0x114, 0x115};
device_t deviceRoles[] = {DEVICE0, DEVICE1, DEVICE2, DEVICE3, DEVICE4, DEVICE5, DEVICE6, DEVICE7};

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
  // Start the GZLL stack
  RFduinoGZLL.hostBaseAddress = hostBaseAddresses[deviceID];
  RFduinoGZLL.begin(HOST);
}

void setupDevice() {
}

void loop() {
  setTime();
  displayClockTime();
  // Time is set
  if (timeIsSet()) {
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
  // Time is not set
  else {
    // Wait for time on Serial Monitor
    delay(500);
  }
}

void loopHost() { 
  if (inDataCollectionPeriod()) {
    // Start GZZL stack from wake cycle
    RFduinoGZLL.hostBaseAddress = hostBaseAddresses[deviceID];
    RFduinoGZLL.begin(HOST);
    
    int i;
    // Reset the RSSI averaging for each device
    for (i = 0; i < MAX_DEVICES; i++) {
      rssi_total[i] = 0;
      rssi_count[i] = 0;
    }
    // Start collecting RSSI samples
    collect_samples = 1;
    // Wait one second
    timeDelay(timeAsHost);
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
    // Find the device with the maximum RSSI value
    /*int closest = 0;
    for (i = 1; i < MAX_DEVICES; i++)
      if (average[i] > average[closest])
        closest = i;
    closest_device = closest;*/
  }
}

void loopDevice() {
  if (inDataCollectionPeriod()) {
    RFduinoBLE.begin();
    // Sleep for timeAsDevice
    RFduino_ULPDelay(MILLISECONDS(timeAsDevice));
    updateTime(timeAsDevice);
    RFduinoBLE.end();
    // Send data to all other hosts
    //pollHost(deviceRole, hostBaseAddresses[deviceID]);
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
    // Wait 200ms
    timeDelay(200);
    RFduinoGZLL.end();
  }
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
  if (deviceRole == HOST) {
    // Ignore device if outside range
    if (device > MAX_DEVICES)
      return;
    if (RFduinoGZLL.hostBaseAddress == hostBaseAddresses[0])
      printf("ID %d received packet from DEVICE%d (hba0) - RSSI: %d\n", deviceID, device, rssi);
    if (RFduinoGZLL.hostBaseAddress == hostBaseAddresses[1])
      printf("ID %d received packet from DEVICE%d (hba1) - RSSI: %d\n", deviceID, device, rssi);
    // If collecting samples, update the RSSI total and count
    if (collect_samples) {
        rssi_total[device] += rssi;
        rssi_count[device]++;
    }
  }
}

void timeDelay(int ms) {
  delay(ms);
  updateTime(ms);
}

void setTime() {
  // Set local time from Serial Monitor
  if (Serial.available() > 0) {
    timer.hour = Serial.parseInt();
    timer.minute = Serial.parseInt();
    timer.second = Serial.parseInt();
    timer.ms = Serial.parseInt();
  }
}

boolean timeIsSet() {
  return !(timer.hour == 0 && timer.minute == 0);
}

void updateTime(int ms) {
  // Update time only if its been set
  if (!(timer.hour == 0 && timer.minute == 0 && timer.second == 0 && timer.ms == 0)) {
    int carryOver;
    // Update each time component and propagate carryOver linearly
    timer.ms += ms;
    carryOver = floor(timer.ms/1000);
    if (timer.ms > 999) {
      timer.ms = timer.ms % 1000;
    }
    timer.second += carryOver;
    carryOver = floor(timer.second/60);
    if (timer.second > 59) {
      timer.second = timer.second % 60;
    }
    timer.minute += carryOver;
    carryOver = floor(timer.minute/60);
    if (timer.minute > 59) {
      timer.minute = timer.minute % 60;
    }
    timer.hour += carryOver;
    carryOver = floor(timer.hour/24);
    if (timer.hour > 23) {
      timer.hour = timer.hour % 24;
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
  if (timer.hour >= START_HOUR && timer.hour <= END_HOUR) {
    // Inclusive on START_MINUTE
    if (timer.hour == START_HOUR && timer.minute >= START_MINUTE) {
      return true;
    }
    // Exclusive on END_MINUTE
    if (timer.hour == END_HOUR && timer.minute < END_MINUTE) {
      return true;
    }
  }
  // Otherwise, do not collect data
  else {
    return false;
  }
}

void displayClockTime() {
  displayDate();
  Serial.print(timer.hour);
  Serial.print(":");
  Serial.print(timer.minute);
  Serial.print(":");
  Serial.print(timer.second);
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
  int ms = 1000 - timer.ms;
  int seconds = 60 - timer.second;
  int minutes = (60 - timer.minute + START_MINUTE) % 60;
  int hours = (timer.hour <= START_HOUR) ? START_HOUR - timer.hour - 1 : 24 - timer.hour + START_HOUR - 1;
  // Calculate time to START_HOUR:START_MINUTE by converting higher order time units to ms
  int delayTime = ms + 1000*seconds + 60000*minutes + 3600000*hours;
  // Add additional offset for each device
  //int offset = 60000*deviceID;
  //delayTime += offset;
  Serial.print("Sleeping for ");
  Serial.print(delayTime);
  Serial.println(" ms");
  timeDelay(delayTime);
}

