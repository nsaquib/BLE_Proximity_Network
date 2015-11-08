/*
 * Region Tracking Device -- keep a few hosts around the desired area who will continually listen
 * for other beacons that people are carrying around.
 * 
 * Social Computing Group, MIT Media Lab
 */

#include <RFduinoGZLL.h>
#include <RFduinoBLE.h>

#define DEVICE_LOOP_TIME 1000

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

// pin for the Green Led
int green_led = 3;

void setup() {
  pinMode(green_led, OUTPUT);
  RFduinoGZLL.txPowerLevel = 4;
  RFduinoGZLL.hostBaseAddress = 0x000;
  RFduinoGZLL.begin(role);
}

void loop() {
  delay(DEVICE_LOOP_TIME);
  pollHost();
}

void pollHost() {
  // Only a device can poll the host
  if (deviceRole != HOST) {
    // Send deviceID to host
    RFduinoGZLL.sendToHost(deviceID);
  }
}

device_t assignDeviceT() {
  int deviceNum = (int) (deviceID % 8);
  return deviceRoles[deviceNum];
}
