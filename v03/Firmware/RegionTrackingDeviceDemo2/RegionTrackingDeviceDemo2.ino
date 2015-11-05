/*
 * PrNet demo -- keep a few hosts around the desired area who will continually listen
 * for other beacons that people are carrying around.
 * 
 * Nazmus Saquib
 * Social Computing Group, MIT Media Lab
 */

#include <RFduinoGZLL.h>
#include <RFduinoBLE.h>

#define sleep_time 5000

const int deviceID = 0;

device_t role = DEVICE0;
const int HBAs[] = {0x000, 0x001, 0x002, 0x003, 0x004, 0x005}; //0x002, 0x003, 0x004, 0x005, 0x006, 0x007};
const int HOSTS = sizeof(HBAs)/sizeof(*HBAs);

// pin for the Green Led
int green_led = 3;

void setup() {
  pinMode(green_led, OUTPUT);
  RFduinoGZLL.txPowerLevel = 4;
  RFduinoGZLL.begin(role);
}

void loop() {
  //delay(sleep_time);
  //RFduinoGZLL.sendToHost(NULL, 0);
  pollAllHosts();
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
}

void pollAllHosts() {
  int i;
  for (i = 0; i < HOSTS; i++) {
    pollHost(HBAs[i]);
  }
  delay(sleep_time);
}

void pollHost(int hostAddr) {
  RFduinoGZLL.hostBaseAddress = hostAddr;
  RFduinoGZLL.sendToHost(NULL, 0);
  delay(200);
}

