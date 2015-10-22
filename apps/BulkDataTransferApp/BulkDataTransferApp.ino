
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

// we can hold 80 rows of 12 bytes data (t, id, rssi) to be under 1K page memory
#define lenrec 80

int stop_len = STORAGE_FLASH_PAGE - 30;

// send 500 20 byte buffers = 10000 bytes
int packets = 5;

// flag used to start sending
int flag = false;

// variables used in packet generation
int ch;
int packet;
int start;

PrNetRomManager m;

void setup() {
  //Serial.begin(57600);
  RFduinoBLE.advertisementData = "Ayesha"; // shouldnt be more than 10 characters long
  RFduinoBLE.deviceName = "Ayesha device";  //  name of your RFduino. Will appear when other BLE enabled devices search for it
  RFduinoBLE.begin(); // begin
  //RFduino_ULPDelay(INFINITE);
}

void RFduinoBLE_onReceive(char *data, int len)
{
  // if the first byte is 0x01 / on / true
  if (!data[0])
  {
    RFduinoBLE.send(0);
    packet = 0;
    // ch = 'A';
    start = 0;
    flag = true;
    startTransfer();
  }
  else 
  {
    RFduinoBLE.send(1);
    //flag = false;
  }
}

void startTransfer() {
  Serial.begin(9600);
  Serial.println("STARTING TRANSFER");
  int i = STORAGE_FLASH_PAGE;
  while (flag)
  {
    m.loadPage(i);
    Serial.println("Starting page: ");
    Serial.println(i);

    char buf_t[lenrec];
    char buf_id[lenrec];
    char buf_rsval[lenrec];
    
    // generate the next packet
    for (int j = 0; j < lenrec; j++)
    {
      sprintf(buf_t, "%d", m.table.t[j]);
      
      sprintf(buf_id, "%d", m.table.id[j]);

      sprintf(buf_rsval, "%d", m.table.rsval[j]);
    }
    Serial.println("Finished copying");
    
    // send is queued (the ble stack delays send to the start of the next tx window)
    while (! RFduinoBLE.send(buf_t, 20)){
      //Serial.print(".");
    }
    Serial.flush();
      ;  // all tx buffers in use (can't send - try again later)
    Serial.println("Sent t");
    Serial.println(buf_t);
    delay(20); 
    
    // send is queued (the ble stack delays send to the start of the next tx window)
    //while (! RFduinoBLE.send(buf_id, 20))
    //;  // all tx buffers in use (can't send - try again later)
    Serial.println("Sent id");
    Serial.println(buf_id);
    delay(20); 
    
    // send is queued (the ble stack delays send to the start of the next tx window)
    //while (! RFduinoBLE.send(buf_rsval, 20))
    //;  // all tx buffers in use (can't send - try again later)
    Serial.println("Sent rsval");
    Serial.println(buf_rsval);
    delay(2000); 
    
    Serial.println("Finished with loop");
    if (! start)
      start = millis();
      
    i--;
    if (i < stop_len)
    {
      Serial.println("Ending");
      int end = millis();
      float secs = (end - start) / 1000.0;
      int bps = ((packets * 20) * 8) / secs; 
      //RFduinoBLE.send("Finished", 8);
      //RFduinoBLE.send(start);
      //RFduinoBLE.send(end);
      //RFduinoBLE.send(secs);
      //RFduinoBLE.send(bps / 1000.0);
      //RFduinoBLE.send(2);
      flag = false;
      RFduinoBLE.send(0);
      RFduino_ULPDelay(INFINITE);
    }
  }
}
 
void loop() {
  //RFduino_ULPDelay(INFINITE);
}
