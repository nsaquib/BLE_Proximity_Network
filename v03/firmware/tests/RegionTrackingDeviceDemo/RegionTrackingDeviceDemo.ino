/*
 * PrNet demo -- keep a few hosts around the desired area who will continually listen
 * for other beacons that people are carrying around.
 * 
 * Nazmus Saquib
 * Social Computing Group, MIT Media Lab
 */

#include <RFduinoGZLL.h>
#include <RFduinoBLE.h>

#define sleep_time 2000

device_t role = DEVICE0;

// pin for the Green Led
int green_led = 3;

void setup()
{
  pinMode(green_led, OUTPUT);

    // the host/device base address can be changed to create independent networks in the same area
  // (note: the msb cannot be 0x55 or 0xaa)
  //RFduinoGZLL.hostBaseAddress = 0x12345678;    // default host base address is 0x0D0A0704;
  //RFduinoGZLL.deviceBaseAddress = 0x87654321;  // default device base address is 0x0E0B0805;

  //RFduinoGZLL.begin(role); // consumes a few mA

  RFduinoGZLL.txPowerLevel = 4;

  RFduinoGZLL.begin(role);

}

void loop()
{
  
  ///*

  //RFduinoBLE.begin();
  //RFduino_ULPDelay(sleep_time);
  //RFduinoBLE.end();

  delay(sleep_time);
  //pollHost(role);

  
  
  RFduinoGZLL.sendToHost(NULL, 0);

  //delay(200);

  //RFduinoGZLL.end();

/*
  RFduinoGZLL.hostBaseAddress = 0x12345677;
  RFduinoGZLL.begin(role);

  // request the state from the Host (send a 0 byte payload)
  // pole different hosts
  
  RFduinoGZLL.sendToHost(NULL, 0);

  delay(200);
  
  RFduinoGZLL.end();
  
  RFduinoGZLL.hostBaseAddress = 0x12345678;
  RFduinoGZLL.begin(role);
  RFduinoGZLL.sendToHost(NULL, 0);

  delay(200);

  RFduinoGZLL.end();
*/
  
  //*/
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len)
{
  /*
  // ignore acknowledgement without payload
  if (len > 0)
  {
    // set the Green led if this device is the closest device
    device_t closest_device = (device_t)data[0];
    digitalWrite(green_led, (role == closest_device));
    Serial.println(data[0]);
  }
  */
}

void pollHost(device_t drole)
{
  //RFduinoGZLL.hostBaseAddress = hostaddr;
  RFduinoGZLL.begin(drole);
  
  RFduinoGZLL.sendToHost(NULL, 0);

  delay(200);

  RFduinoGZLL.end();
}

