/*
 * PrNet Host Demo
 * Dwyane George
 * Social Computing Group, MIT Media Lab
 */

#define MAX_DEVICES 8
#define DATA_COLLECTION_PERIOD 500 // in milliseconds

#include <RFduinoGZLL.h>

// RSSI total and count for each device for averaging
int rssi_total[MAX_DEVICES];
int rssi_count[MAX_DEVICES];

// collect samples flag
int collect_samples = 0;
bool sleep_flag = false;
const int HBA = 0x000;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  RFduinoGZLL.txPowerLevel = 4;
}

void loop() {
  RFduinoGZLL.hostBaseAddress = HBA;
  RFduinoGZLL.begin(HOST);
  int i;
  // reset the RSSI averaging for each device
  for (i = 0; i < MAX_DEVICES; i++) {
    rssi_total[i] = 0;
    rssi_count[i] = 0;
  }

  // start collecting RSSI samples
  collect_samples = 1;

  // wait a few seconds
  delay(DATA_COLLECTION_PERIOD);
  
  // stop collecting RSSI samples
  collect_samples = 0;

  // calculate the RSSI avarages for each device
  int average[MAX_DEVICES];
 
  for (i = 0; i < MAX_DEVICES; i++) {
    // no samples received, set to the lowest RSSI
    // (also prevents divide by zero)
    if (rssi_count[i] == 0)
      average[i] = -128;
    else
      average[i] = rssi_total[i] / rssi_count[i];

    Serial.print(i);
    Serial.print(",");
    Serial.print(average[i]);
    Serial.print(",");
    Serial.println(rssi_count[i]);
  }
  RFduinoGZLL.end();
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
  // ignore device if outside range
  if (device > MAX_DEVICES)
    return;
    
  // if collecting samples, update the RSSI total and count
  if (collect_samples) {
    rssi_total[device] += rssi;
    rssi_count[device]++;
  }
}
