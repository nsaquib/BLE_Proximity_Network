/*
 * Region Tracking Host -- keep a few hosts around the desired area who will continually listen
 * for other beacons that people are carrying around.
 * 
 * Social Computing Group, MIT Media Lab
 */

#define MAX_DEVICES 10
#define MAX_ROWS 240
#define HOST_LOOP_TIME 1000
#define HOST_SLEEP_TIME 200

#include <RFduinoBLE.h>
#include <RFduinoGZLL.h>
#include <PrNetRomManager.h>

// RSSI total and count for each device for averaging
float rssiTotal[MAX_DEVICES];
float rssiCount[MAX_DEVICES];
float rssiAverages[MAX_DEVICES];

// Collect samples flag
int collectSamples = 0;
// Counter for the number of nonzero rows in the current PrNetRomManager table
int rowCounter = 0;
// Time elapsed
long timePassed = 0; // in milliseconds
// Designated time to run
const long timeToRun = 10000; // 4 hours in milliseconds
// Host base address
const int HBA = 0x000;
// Pin for the green LED
int greenLED = 3;

bool transfer_flag = false;
bool start = false;

PrNetRomManager m;
PrNetRomManager m2; // for reading flash ROM and sending through BLE
int page_counter = STORAGE_FLASH_PAGE;
int stop_len = STORAGE_FLASH_PAGE - 49;

void setup() {
  pinMode(greenLED, OUTPUT);
  RFduinoGZLL.txPowerLevel = 4;
  RFduinoGZLL.hostBaseAddress = 0x000;
  RFduinoGZLL.begin(HOST);
  Serial.begin(9600);
}

void loop() {
  while (!start) {
    Serial.println("Waiting for start from phone app...");
    setStart();
  }
  if (timePassed < timeToRun) {
    Serial.println("My role is HOST");
    resetRSSI();
    collectSamplesFromDevices();
    calculateRSSIAverages();
    updateROMTable();
    sleepHost(HOST_SLEEP_TIME);
  } else {
    sleepForever();
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
    } else {
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
  delay(HOST_LOOP_TIME);
  timePassed += HOST_LOOP_TIME;
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

void sleepHost(int milliseconds) {
  // sleep for some time
  timePassed += milliseconds;
  Serial.println("Entering sleep");
  RFduinoGZLL.end();
  RFduinoBLE.begin();
  RFduino_ULPDelay(milliseconds);
  RFduinoBLE.end();
  RFduinoGZLL.begin(HOST);
}

void sleepForever() {
  // Sleep host indefinitely
  Serial.println("Sleeping forever...");
  RFduinoGZLL.end();
  RFduinoBLE.begin();
  RFduino_ULPDelay(INFINITE);
  RFduinoBLE.end();
  RFduinoGZLL.begin(HOST);
}

////////// Callback Functions //////////

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
  // Ignore device if outside range, should never occur
  // If collecting samples, update the RSSI total and count
  if (collectSamples && (int) data[0] >= 0 && (int) data[0] < MAX_DEVICES) {
    rssiTotal[(int) data[0]] += rssi;
    rssiCount[(int) data[0]]++;
  }
}

////////// ROM Code //////////

void updateROMTable() {
  // Update rows for rom table
  int i;
  for (i = 0; i < MAX_DEVICES; i++) {
    if (rssiAverages[i] > -100) {
      if (rowCounter >= MAX_ROWS) {
        writePage();
      }
      int data = millis()/1000;
      data += abs(int(rssiAverages[i])) * 1000000;
      data += i * 100000000;
      m.table.t[rowCounter] = data;
      rowCounter++;
    }
  }
}

void writePage() {
  // Erase any existing data in the current rom page
  m.erasePage(STORAGE_FLASH_PAGE - m.pagecounter);
  // Write to rom memory
  int success = m.writePage(STORAGE_FLASH_PAGE - m.pagecounter, m.table);
  Serial.println(success);
  rowCounter = 0;
}

////////// App Integration Code //////////

void setStart() {
  RFduinoGZLL.end();
  RFduinoBLE.begin();
  // Set local time from phone app
  while (!start) {
    delay(100);
    if (transfer_flag) {
      startTransfer();
    }
  }
  RFduinoBLE.end();
}

void RFduinoBLE_onReceive(char *data, int len) {
  //Serial.println(data);
  // if the first byte is 0x01 / on / true
  if (data[0])
  {
    //Serial.println(data[0]);
    if(data[0] == '%')
    {
      start = true;
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
      while (! RFduinoBLE.send(buf_t, 10));
      //while (! RFduinoBLE.send(space));
      delay(5); 
      while (! RFduinoBLE.send(buf_id, 10));
      //while (! RFduinoBLE.send(space));
      delay(5);
      while (! RFduinoBLE.send(buf_rsval, 10));
      //while (! RFduinoBLE.send(space));
      while (! RFduinoBLE.send('|'));
    } 
    Serial.println("Finished with loop");
    while (! RFduinoBLE.send('#'));
    delay(200);
    page_counter--;
    if (page_counter < stop_len)
    {
      Serial.println("Ending");
      transfer_flag = false;
      RFduinoBLE.send(0);
    }
  }
}
