/*
 * PrNet Device Demo
 * Dwyane George
 * Social Computing Group, MIT Media Lab
 */

#define sleep_time 50 // 200 milliseconds

#include <RFduinoGZLL.h>

device_t role = DEVICE0;
const int HBA = 0x000;

// pin for the Green Led
int green_led = 3;

void setup() {
  pinMode(green_led, OUTPUT);
  RFduinoGZLL.txPowerLevel = 4;
  RFduinoGZLL.hostBaseAddress = HBA;
  RFduinoGZLL.begin(role);
}

void loop() {
  pollHost(role);
}

void pollHost(device_t drole) {
  RFduinoGZLL.sendToHost(NULL, 0);
  delay(sleep_time);
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
}
