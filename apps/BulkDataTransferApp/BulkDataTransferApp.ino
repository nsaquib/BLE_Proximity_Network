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
  Serial.begin(57600);
  RFduinoBLE.advertisementData = "Ayesha"; // shouldnt be more than 10 characters long
  RFduinoBLE.deviceName = "Ayesha device";  //  name of your RFduino. Will appear when other BLE enabled devices search for it
  RFduinoBLE.begin(); // begin
}

void RFduinoBLE_onConnect() {
  packet = 0;
  ch = 'A';
  start = 0;
  flag = true;
  RFduinoBLE.send("Sending", 7);
  // first send is not possible until the iPhone completes service/characteristic discovery
}

void RFduinoBLE_onReceive(char *data, int len)
{
  // if the first byte is 0x01 / on / true
  if (data[0])
  {
    packet = 0;
    ch = 'A';
    start = 0;
    flag = true;
    RFduinoBLE.send("Sending", 7);
    RFduinoBLE.send(1);
  }
  else
    RFduinoBLE.send(0);
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
      flag = false;
    }
  }
}
