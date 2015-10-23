
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
#include <PrNetRomManager.h>

#define lenrec 80

int stop_len = STORAGE_FLASH_PAGE - 60;

// flag used to start sending
int flag = false;

int device_id = 5;

PrNetRomManager m;

int i = STORAGE_FLASH_PAGE;

void setup() {
  Serial.begin(9600);
  RFduinoBLE.advertisementData = "Ayesha"; // shouldnt be more than 10 characters long
  char deviceName[15];
  String device_str = String(device_id);
  device_str.toCharArray(deviceName, 15);
  RFduinoBLE.deviceName = deviceName; //  name of your RFduino. Will appear when other BLE enabled devices search for it
  RFduinoBLE.begin(); // begin
}

void RFduinoBLE_onReceive(char *data, int len)
{
  // if the first byte is 0x01 / on / true
  if (data[0])
  {
    //Serial.println(data[0]);
    if(data[0] == '>')
    {
      //UPDATE START TIME
      while(! RFduinoBLE.send(data, len));
    }
    else if(data[0] == '<')
    {
      //UPDATE END TIME
      while(! RFduinoBLE.send(data, len));
    }
    else{
      RFduinoBLE.send(1);
      flag = true;
    }
  }
  else 
  {
    RFduinoBLE.send(0);
    flag = false;
  }
}
 
void loop() {
  while (flag)
  {
    m.loadPage(i);
    Serial.println("Starting page: ");
    Serial.println(i);
    
    // generate the next packet
    for (int j = 0; j < lenrec; j++)
    {
      char space = ' ';
      char buf_t[10];
      char buf_id[10];
      char buf_rsval[10];
      sprintf(buf_t, "%d", m.table.t[j]);
      sprintf(buf_id, "%d", m.table.id[j]);
      sprintf(buf_rsval, "%d", m.table.rsval[j]);
      while (! RFduinoBLE.send(buf_t, 10));
      //Serial.println("Sent t");
      //Serial.println(buf_t);
      while (! RFduinoBLE.send(space));
      delay(20); 
      while (! RFduinoBLE.send(buf_id, 10));
      //Serial.println("Sent id");
      //Serial.println(buf_id);
      while (! RFduinoBLE.send(space));
      delay(20);
      while (! RFduinoBLE.send(buf_rsval, 10));
      //Serial.println("Sent rsval");
      //Serial.println(buf_rsval);
      //delay(2000); 
      while (! RFduinoBLE.send(space));
      while (! RFduinoBLE.send('|'));
    } 
    Serial.println("Finished with loop");
    while (! RFduinoBLE.send('#'));
    delay(200);
    i--;
    if (i < stop_len)
    {
      Serial.println("Ending");
      flag = false;
      RFduinoBLE.send(0);
      //RFduino_ULPDelay(INFINITE);
    }
  }
}
