/*
 * Region Tracking Device -- keep a few hosts around the desired area who will continually listen
 * for other beacons that people are carrying around.
 * 
 * Social Computing Group, MIT Media Lab
 */

#include <RFduinoGZLL.h>
#include <RFduinoBLE.h>

#define DEVICE_LOOP_TIME 100

/*
 * ID DEVICEX
 *  0 DEVICE0
 *  1 DEVICE1
 *  2 DEVICE2
 *  ...
 */
const int deviceID = 0;
// Device roles
const device_t deviceRoles[] = {DEVICE0, DEVICE1, DEVICE2, DEVICE3, DEVICE4, DEVICE5, DEVICE6, DEVICE7};
device_t role = assignDeviceT();
// Time elapsed
long timePassed = 0; // in milliseconds
// Designated time to run
const long timeToRun = 14400000; // 4 hours in milliseconds
// Host base address
const int HBA = 0x000;
// pin for the Green Led
int greenLED = 3;

void setup() {
  pinMode(greenLED, OUTPUT);
  RFduinoGZLL.txPowerLevel = 4;
  RFduinoGZLL.hostBaseAddress = HBA;
  RFduinoGZLL.begin(role);
  Serial.begin(9600);
}

void loop() {
  if (timePassed < timeToRun) {
    Serial.print("My role is DEVICE");
    Serial.println(role);
    pollHost();
    //sleepDevice(DEVICE_LOOP_TIME);
    timeDelay(DEVICE_LOOP_TIME);
  } else {
    sleepForever();
  }
}

////////// Device Functions //////////

void pollHost() {
  // Send deviceID to host
  RFduinoGZLL.sendToHost(deviceID);
}

device_t assignDeviceT() {
  int deviceNum = (int) (deviceID % 8);
  return deviceRoles[deviceNum];
}

void timeDelay(int milliseconds) {
  timePassed += milliseconds;
  delay(milliseconds);
}

////////// Sleep Functions //////////

void sleepDevice(int milliseconds) {
  // Sleep for some time
  timePassed += milliseconds;
  Serial.println("Entering sleep");
  RFduinoGZLL.end();
  RFduinoBLE.begin();
  RFduino_ULPDelay(milliseconds);
  RFduinoBLE.end();
  RFduinoGZLL.begin(role);
}

void sleepForever() {
  // Sleep host indefinitely
  Serial.println("Sleeping forever...");
  RFduinoGZLL.end();
  RFduinoBLE.begin();
  RFduino_ULPDelay(INFINITE);
  RFduinoBLE.end();
  RFduinoGZLL.begin(role);
}
