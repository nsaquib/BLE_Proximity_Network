#include <PrNetRomManager.h>
#include <SimbleeCOM.h>
#include <Time.h>
#include <Wire.h>

#define NETWORK_SIZE        13
#define TX_POWER_LEVEL      -4
#define BAUD_RATE           57600
#define USE_SERIAL_MONITOR  true
#define USE_PROXMITY_MODE   false

#define START_HOUR          0
#define START_MINUTE        0
#define END_HOUR            24
#define END_MINUTE          0

#define INTERRUPT_PIN       2
#define DS3231_ADDR         0x68
#define CTRL_REG            0x0E
#define START_INT           0b01000000 
#define STOP_INT            0b00000100

int seconds[NETWORK_SIZE];
bool printSeconds;

// Time keeping data structure
Time timer;
// Unique device ID
int deviceID = 11;
// Data to transmit
char payload[] = { deviceID };
// RSSI total and count for each device
int rssiTotal[NETWORK_SIZE];
int rssiCount[NETWORK_SIZE];
// ROM Manager data structure
PrNetRomManager romManager;
// Counter for current ROM page
int pageCounter = STORAGE_FLASH_PAGE;
// Counter for current ROM page row
int rowCounter;
// Boolean on if received new data
bool newData;
// Boolean on whether to collect data
bool collectData;
// Boolean on if radio stack is on
bool broadcasting;

void setup() {
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  Simblee_pinWakeCallback(INTERRUPT_PIN, HIGH, onInterrupt);
  randomSeed(analogRead(0));
  communicateOverSerial();
  enableSerialMonitor();
  SimbleeCOM.txPowerLevel = TX_POWER_LEVEL;
}

void loop() {
  if (collectData) {// && timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE)) {
  startBroadcast();
  delay(random(5, 100));
  char data[] = {deviceID, (timer.initialTime.seconds + timer.secondsElapsed) % 10};
  SimbleeCOM.send(data, sizeof(data));
  } else {
    stopBroadcast();
    updateROM();
    Simblee_ULPDelay(INFINITE);
  }
  
  if (printSeconds) {
    for (int i = 0; i < NETWORK_SIZE; i++) {
      Serial.print(String(i) + "\t");
    }
    Serial.println();
    for (int i = 0; i < NETWORK_SIZE; i++) {
      if (deviceID == i) {
        Serial.print(String((timer.initialTime.seconds + timer.secondsElapsed) % 10) + "\t");
      } else {
        Serial.print(String(seconds[i]) + "\t");
      }
    }
    Serial.println();
    printSeconds = false;
  }
}

int onInterrupt(uint32_t ulPin) {
  timer.secondsElapsed++;
  collectData = (timer.initialTime.seconds + timer.secondsElapsed) % 10 < 5;
  printSeconds = true;
  timer.displayDateTime();
  return collectData;
}

void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi) {
  if ((int) payload[0] >= 0 && (int) payload[0] < NETWORK_SIZE) {
    rssiTotal[(int) payload[0]] += rssi;
    rssiCount[(int) payload[0]]++;
    seconds[(int) payload[0]] = payload[1];
    newData = true;
  }
}

////////// Time Functions //////////

/*
 * Sets system time sent from serial monitor
 */
void programSystemTime() {
  char systemTime[16];
  Serial.readBytes(systemTime, 16);
  int currentMilliseconds = (systemTime[13] - '0') * 100 + (systemTime[14] - '0') * 10 + (systemTime[15] - '0');
  delay((1000 - currentMilliseconds) % 1000);
  startInterrupt();
//  timer.setInitialTime(0, 0, 0, 0, 0, 0, 0);
  timer.setInitialTime(
    (systemTime[0] - '0') * 10 + (systemTime[1] - '0'),
    (systemTime[2] - '0') * 10 + (systemTime[3] - '0'),
    (systemTime[4] - '0') * 10 + (systemTime[5] - '0'),
    (systemTime[6] - '0'),
    (systemTime[7] - '0') * 10 + (systemTime[8] - '0'),
    (systemTime[9] - '0') * 10 + (systemTime[10] - '0'),
    (systemTime[11] - '0') * 10 + (systemTime[12] - '0'));
  Serial.println(systemTime);
  Serial.println("Received time.");
  Serial.end();
}

////////// Radio Functions //////////

/*
 * Enables radio broadcasting
 */
void startBroadcast() {
  if (!broadcasting) {
//    Serial.println("On");
    SimbleeCOM.begin();
    broadcasting = true;
    writeTimeROMRow();
  }
}

/*
 * Disables radio broadcasting
 */
void stopBroadcast() {
  if (broadcasting) {
//    Serial.println("Off");
    SimbleeCOM.end();
    broadcasting = false;
  }
}

/*
 * Enables proximity mode
 */
void enableProximityMode() {
  if (USE_PROXMITY_MODE) {
    SimbleeCOM.proximityMode(true);
  }
}

////////// ROM Functions //////////

/*
 * Stores deviceID (I), RSSI (R), and time (T) as I,IRR,TTT,TTT with maximum value of 4,294,967,295, (2^32 - 1)
 */
void updateROM() {
  if (newData) {
    struct currentTime t = timer.getTime();
    int data = (t.seconds % 60) + ((t.minutes % 60) * 100) + ((t.hours % 24) * 10000);
    for (int i = 0; i < NETWORK_SIZE; i++) {
      int rssiAverage = (rssiCount[i] == 0) ? -128 : rssiTotal[i] / rssiCount[i];
      Serial.println(String(i) + "\t" + String(rssiAverage) + "\t" + String(rssiCount[i]));
      if (rssiAverage > -100) {
        if (rowCounter >= MAX_ROWS) {
          writePage();
        }
        romManager.table.data[rowCounter] = data + (abs(rssiAverage % 100) * 1000000) + ((i % 42) * 100000000);
        rowCounter++;
      }
      rssiTotal[i] = 0;
      rssiCount[i] = 0;
    }
    writePartialPage(false);
  }
  newData = false;
}

/*
 * Write a timestamp row HH:MM:SS to the ROM table
 */
void writeTimeROMRow() {
  if (rowCounter >= MAX_ROWS) {
    writePage();
  }
  struct currentTime t = timer.getTime();
  romManager.table.data[rowCounter] = (t.seconds % 60) + ((t.minutes % 60) * 100) + ((t.hours % 24) * 10000);
  rowCounter++;
}

/*
 * Persists the table to nonvolatile ROM memory
 */
void writePage() {
  int success = romManager.writePage(STORAGE_FLASH_PAGE - romManager.pageCounter, romManager.table);
  Serial.println(success);
  rowCounter = 0;
}

/*
 * Persists a partially filled table to nonvolatile ROM memory
 */
void writePartialPage(bool advanceRowCounter) {
  for (int i = rowCounter; i < MAX_ROWS; i++) {
    romManager.table.data[i] = -1;
  }
  int success = romManager.writePartialPage(STORAGE_FLASH_PAGE - romManager.pageCounter, romManager.table);
  Serial.println(success);
  if (advanceRowCounter) {
    rowCounter++;
  }
}

/*
 * Erases all existing ROM data
 */
void eraseROM() {
  for (int i = STORAGE_FLASH_PAGE; i > STORAGE_FLASH_PAGE - 128; i--) {
    romManager.erasePage(i);
  }
  Serial.println("Erased ROM");
}

/*
 * Prints all existing ROM data from page 250 to page 124
 */
void printROM() {
  for (int i = STORAGE_FLASH_PAGE; i >= 124; i--) {
    romManager.printPage(i);
  }
  Serial.println("Printed ROM");
}

////////// RTC Functions //////////

/*
 * Starts the RTC interrupt
 */
void startInterrupt() {
  Wire.begin();
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(CTRL_REG);
  Wire.write(START_INT);
  Wire.endTransmission();
}

/*
 * Stops the RTC interrupt
 */
void stopInterrupt() {
  Wire.begin();
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(CTRL_REG);
  Wire.write(STOP_INT);
  Wire.endTransmission();
}

////////// Serial Monitor Functions //////////

/*
 * Enables the serial monitor
 */
void enableSerialMonitor() {
  if (USE_SERIAL_MONITOR) {
    Serial.begin(BAUD_RATE);
  }
}

/*
 * Disables the serial monitor
 */
void disableSerialMonitor() {
  if (USE_SERIAL_MONITOR) {
    Serial.end();
  }
}

/*
 * Interfaces with device over serial connection to send system time or receive ROM data
 */
void communicateOverSerial() {
  stopInterrupt();
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(5);
  Serial.println("Waiting for serial communication...");
  while (true) {
    delay(5);
    char c = Serial.read();
    if (c == 'P') {
      printROM();
    } else if (c == 'T') {
      programSystemTime();
      break;
    }
  }
}
