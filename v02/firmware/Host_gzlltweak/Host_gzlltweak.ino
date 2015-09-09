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

#define  MAX_DEVICES  8

#include <RFduinoGZLL.h>

// RSSI total and count for each device for averaging
int rssi_total[MAX_DEVICES];
int rssi_count[MAX_DEVICES];

// device with the maximum RSSI
char closest_device = 0;

// collect samples flag
int collect_samples = 0;

void setup()
{
  // write the Device and RSSI values to the Serial Monitor
  Serial.begin(57600);

    // the host/device base address can be changed to create independent networks in the same area
  // (note: the msb cannot be 0x55 or 0xaa)
  //*** try different host addresses to create two star networks
  //RFduinoGZLL.hostBaseAddress = 0x12345678;    // default host base address is 0x0D0A0704;
  //RFduinoGZLL.deviceBaseAddress = 0x87654321;  // default device base address is 0x0E0B0805;
  
  // start the GZLL stack
  RFduinoGZLL.begin(HOST);  

  pinMode(3, OUTPUT);
}

void loop()
{
  //*** for some reason, I have to begin the gzll stack
  //in every loop, otherwise it can't listen to device signals
  //when the device goes to sleep and wakes up
  RFduinoGZLL.hostBaseAddress = 0x123;
  RFduinoGZLL.begin(HOST);  
  
  int i;
  
  // reset the RSSI averaging for each device
  for (i = 0; i < MAX_DEVICES; i++)
  {
    rssi_total[i] = 0;
    rssi_count[i] = 0;
  }

  // start collecting RSSI samples
  collect_samples = 1;

  // wait one second
  delay(1000);
  
  // stop collecting RSSI samples
  collect_samples = 0;

  // calculate the RSSI avarages for each device
  int average[MAX_DEVICES];
 
  for (i = 0; i < MAX_DEVICES; i++)
  {
    // no samples received, set to the lowest RSSI
    // (also prevents divide by zero)
    if (rssi_count[i] == 0)
      average[i] = -128;
    else
      average[i] = rssi_total[i] / rssi_count[i];

    // printf increases the sketch size more than Serial.println, but
    // this is an easy way to concatenate data onto a single line
    // (note: the newline is required with printf!)
    //printf("average RSSI for device %d is %d\n", i, average[i]);
  }
  //Serial.println("");
  
  // find the device with the maximum RSSI value
  int closest = 0;
  for (i = 1; i < MAX_DEVICES; i++)
    if (average[i] > average[closest])
      closest = i;

  closest_device = closest;
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len)
{
  // ignore device if outside range
  if (device > MAX_DEVICES)
    return;

  //Serial.println(data);
  printf("%d: %d\n",device, rssi);
  Serial.println("");
  // if collecting samples, update the RSSI total and count
  if (collect_samples)
  {
    //Serial.println(rssi);
    rssi_total[device] += rssi;
    rssi_count[device]++;
  }
  
  // piggyback max_device on the acknowledgement sent back to the requesting Device
  //RFduinoGZLL.sendToDevice(device, closest_device);
}
