/*
This sketch demonstrates how to coordinate data
between 3 devices in a Gazell network.

The host collects RSSI samples from the Devices,
and determines which device has the strongest
average RSSI (ie: the Device that is closest
to the Host).  The Green led is set on the
closest Device.

Since the Device must initiate communication, the
device "polls" the Host evey 200ms.
*/

#define MAX_DEVICES 16
#define MAX_HOSTS 8

#include <RFduinoGZLL.h>
#include <RFduinoBLE.h>

// Device ID: 0...N
int deviceID = 1;
device_t deviceRole = HOST;

// Device roles, host base addresses, and device base addresses
int hostBaseAddresses[] = {0x000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 0x008, 0x009, 0x010, 0x011, 0x012, 0x013, 0x014, 0x015};
int deviceBaseAddresses[] = {0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x109, 0x110, 0x111, 0x112, 0x113, 0x114, 0x115};
//device_t deviceRoles[] = {DEVICE0, DEVICE1, DEVICE2, DEVICE3, DEVICE4, DEVICE5, DEVICE6, DEVICE7};

// RSSI total and count for each device for averaging
int rssi_total[MAX_DEVICES];
int rssi_count[MAX_DEVICES];

// Device with the maximum RSSI
char closest_device = 0;

// Collect samples flag
int collect_samples = 0;

// Pin for the green LED
int greenLED = 3;

// Counter for tracking host/device roles
int hostTimer = 0;

void setup() {
  pinMode(greenLED, OUTPUT);
  // Adjust power output levels
  RFduinoGZLL.txPowerLevel = 0;
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
  //RFduinoGZLL.deviceBaseAddress = deviceBaseAddresses[deviceID];
  RFduinoGZLL.begin(HOST);
}

void setupDevice() {
}

void loop() {
  // Loop for specific device roles
  if (deviceRole == HOST) {
    loopHost();
  }
  else {
    loopDevice();
  }
}

void loopHost() {
  // Increment host timer
  hostTimer++;
  
  // Start GZZL stack from wake cycle
  RFduinoGZLL.hostBaseAddress = hostBaseAddresses[deviceID];
  //RFduinoGZLL.deviceBaseAddress = deviceBaseAddresses[deviceID];
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
  delay(1000);
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
  int closest = 0;
  for (i = 1; i < MAX_DEVICES; i++)
    if (average[i] > average[closest])
      closest = i;
  closest_device = closest;

  
}

void loopDevice() {
  RFduinoBLE.begin();
  // Wait 3 seconds
  RFduino_ULPDelay(SECONDS(3.0));
  RFduinoBLE.end();
  //delay(3000);
  // Send data to all other hosts
  int i;
  for (i = 0; i < MAX_HOSTS; i++) {
    pollHost(deviceRole, hostBaseAddresses[i]);
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
    delay(200);
    RFduinoGZLL.end();
  }
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
  if (deviceRole == HOST) {
    // Ignore device if outside range
    if (device > MAX_DEVICES)
      return;
    printf("ID %d received packet from DEVICE%d - RSSI: %d\n", deviceID, device, rssi);
    // If collecting samples, update the RSSI total and count
    if (collect_samples) {
        rssi_total[device] += rssi;
        rssi_count[device]++;
    }
  }
  else {
    /*printf("Device - %d: %d\n", device, rssi);
    // Ignore acknowledgement without payload
    if (len > 0)
    {
      // Set the Green led if this device is the closest device
      device_t closest_device = (device_t) data[0];
      digitalWrite(greenLED, (role == closest_device));
      Serial.println(data[0]);
    }*/
  }
}
