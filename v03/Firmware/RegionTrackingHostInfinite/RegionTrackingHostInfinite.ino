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
const int deviceID = 2;
// Device roles
const device_t deviceRoles[] = {DEVICE0, DEVICE1, DEVICE2, DEVICE3, DEVICE4, DEVICE5, DEVICE6, DEVICE7};
device_t role = assignDeviceT();
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
    pollHost();
    timeDelay(DEVICE_LOOP_TIME);
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
  delay(milliseconds);
}

////////// Sleep Functions //////////

void sleepDevice(int milliseconds) {
  Serial.println("Entering sleep");
  RFduinoGZLL.end();
  RFduinoBLE.begin();
  RFduino_ULPDelay(milliseconds);
  RFduinoBLE.end();
  RFduinoGZLL.begin(role);
}
