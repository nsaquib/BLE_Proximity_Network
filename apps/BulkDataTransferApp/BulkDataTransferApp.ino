
/*
This sketch demonstrates how to do error free bulk data
transfer over Bluetooth Low Energy 4.

The data rate should be approximately:
  - 32 kbit/sec at 1.5ft (4000 bytes per second)
  - 24 kbit/sec at 40ft (3000 bytes per second)
  
This sketch sends a fixed number of 20 byte packets to
an Android application.  Each packet is different, so
that the Android application can verify if any data or
packets were dropped.
*/

#include <RFduinoBLE.h>

// send 500 20 byte buffers = 10000 bytes
int packets = 500;

// flag used to start sending
int flag = false;

// variables used in packet generation
int ch;
int packet;

int start;

void setup() {
  //Serial.begin(57600);
  RFduinoBLE.advertisementData = "Ayesha"; // shouldnt be more than 10 characters long
  RFduinoBLE.deviceName = "Ayesha device";  //  name of your RFduino. Will appear when other BLE enabled devices search for it
  RFduinoBLE.begin(); // begin
}

void RFduinoBLE_onReceive(char *data, int len)
{
  // if the first byte is 0x01 / on / true
  if (data[0])
  {
    RFduinoBLE.send(1);
    packet = 0;
    ch = 'A';
    start = 0;
    flag = true;
  }
  else 
  {
    RFduinoBLE.send(0);
    flag = false;
  }
}
 
void loop() {
  if (flag)
  {
    // generate the next packet
    char buf[20];
    for (int i = 0; i < 20; i++)
    {
      buf[i] = ch;
      ch++;
      if (ch > 'Z')
        ch = 'A';
    }
    
    // send is queued (the ble stack delays send to the start of the next tx window)
    while (! RFduinoBLE.send(buf, 20))
      ;  // all tx buffers in use (can't send - try again later)

    if (! start)
      start = millis();

    packet++;
    if (packet >= packets)
    {
      int end = millis();
      float secs = (end - start) / 1000.0;
      int bps = ((packets * 20) * 8) / secs; 
      RFduinoBLE.send("Finished", 8);
      RFduinoBLE.send(start);
      RFduinoBLE.send(end);
      RFduinoBLE.send(secs);
      RFduinoBLE.send(bps / 1000.0);
      //RFduinoBLE.send(2);
      flag = false;
    }
  }
}
