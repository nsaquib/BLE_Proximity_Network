/*
 * RegionTrackingDeviceV2.ino - Runs PrNet nodes as device continuously
 * Created by Dwyane George, December 24, 2015.
 * Social Computing Group, MIT Media Lab
 */

#include <RFduinoGZLL.h>

#define TX_POWER_LEVEL -16
#define HBA 0x000
#define BAUD_RATE 9600
#define DEVICE_LOOP_TIME 100
#define TIME_TO_RUN 14400000 // 4 hours in milliseconds

const int deviceID = 2;
device_t deviceRole = (device_t) (deviceID % 8);
long timePassed = 0;

void setup() {
  RFduinoGZLL.txPowerLevel = TX_POWER_LEVEL;
  RFduinoGZLL.hostBaseAddress = HBA;
  RFduinoGZLL.begin(deviceRole);
  Serial.begin(BAUD_RATE);
}

void loop() {
  if (timePassed < TIME_TO_RUN) {
    Serial.println("DEVICE");
    RFduinoGZLL.sendToHost(deviceID);
    timePassed += DEVICE_LOOP_TIME;
    delay(DEVICE_LOOP_TIME);
  } else {
    sleepForever();
  }
}

void sleepForever() {
  Serial.println("Sleeping forever...");
  RFduinoGZLL.end();
  RFduino_ULPDelay(INFINITE);
}
