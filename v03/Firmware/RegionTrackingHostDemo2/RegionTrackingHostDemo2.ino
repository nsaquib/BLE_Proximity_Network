/*
 * PrNet demo -- keep a few hosts around the desired area who will continually listen
 * for other beacons that people are carrying around.
 * 
 * Nazmus Saquib
 * Social Computing Group, MIT Media Lab
 */

#define  MAX_DEVICES 8

#include <RFduinoBLE.h>
#include <RFduinoGZLL.h>
#include <PrNetRomManager.h>

#define data_collection_period 10000
#define sleep_time 10000

// change the host addresses to match the device host base addresses
#define hostAddr 0x005

// what name the device shows up in the app
#define ble_name "host0"

// RSSI total and count for each device for averaging
int rssi_total[MAX_DEVICES];
int rssi_count[MAX_DEVICES];

// collect samples flag
int collect_samples = 0;
bool ble_setup_flag = false;
long loopcounter = 0;
long rowcounter = 0;
long timePassed = 0; // in seconds
const long timeToRun = 14400; // in seconds

PrNetRomManager m;  //for writing to flash ROM
PrNetRomManager m2; // for reading flash ROM and sending through BLE

// send to phone vars
// flag used to start sending
int flag = false;
int start;

// variables used in packet generation
int ch;
int packet;

// 1K page with 12 bytes rows and 80 rows = 960 bytes.
// so each 1K page has 960/20 = 48 packets.
int packets = 48;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  // start the GZLL stack
  //RFduinoGZLL.begin(HOST);
  RFduinoGZLL.hostBaseAddress = hostAddr;

  //m.
}

void loop() 
{
  if (timePassed < timeToRun) {
    if(loopcounter >= 10)
    {
      // erase any existing data in the current rom page
      m.erasePage(STORAGE_FLASH_PAGE - m.pagecounter);
      //write to rom memory
      int success = m.writePage(STORAGE_FLASH_PAGE - m.pagecounter, m.table);
      Serial.println(success);
      loopcounter = 0;
    }
    
    int i;
  
    Serial.println("beginning as host");
    RFduinoGZLL.begin(HOST);
    // reset the RSSI averaging for each device
    for (i = 0; i < MAX_DEVICES; i++)
    {
      rssi_total[i] = 0;
      rssi_count[i] = 0;
    }
  
    // start collecting RSSI samples
    collect_samples = 1;
  
    // wait a few seconds
    delay(data_collection_period);
    
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
      //printf("average RSSI for device %d is %d with %d pings\n", i, average[i], rssi_count[i]);
    }
    RFduinoGZLL.end();
  
    // update rows for rom table
    for (i = 0; i < MAX_DEVICES; i++)
    {
      m.table.t[i + loopcounter * MAX_DEVICES] = millis();
      m.table.id[i + loopcounter * MAX_DEVICES] = i;
      m.table.rsval[i + loopcounter * MAX_DEVICES] = average[i];
    }
  
    loopcounter++;
    
    // sleep for some time
    Serial.println("entering sleep");
    RFduinoBLE.begin();
    RFduino_ULPDelay(sleep_time);
    RFduinoBLE.end();
  
    timePassed += data_collection_period/1000;
    timePassed += sleep_time/1000;
  }
  else {
    //delay(10000);
    Serial.println("Sleeping forever...");
    /*
    if(ble_setup_flag == true) {
      loopBLE(false);
    }
    else {
      ble_setup_flag = true;
      loopBLE(true);
    }
    */
    
    RFduinoBLE.begin();
    RFduino_ULPDelay(INFINITE);
    //RFduinoBLE.end();
  }
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len)
{
  // ignore device if outside range
  if (device > MAX_DEVICES)
    return;
    
  // if collecting samples, update the RSSI total and count
  if (collect_samples)
  {
    rssi_total[device] += rssi;
    rssi_count[device]++;
  }
  
  // piggyback max_device on the acknowledgement sent back to the requesting Device
  //RFduinoGZLL.sendToDevice(device, closest_device);
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
  while (flag)
  {
    // generate the next packet
    char buf[lenrec*3];
    for (int i = 0; i < lenrec; i++)
    {
      //buf[i] = value.t[i];
    }
    for (int i = lenrec; i < lenrec*2; i++)
    {
      //buf[i] = value.t[i];
    }
    for (int i = lenrec*2; i < lenrec*3; i++)
    {
      //buf[i] = value.t[i];
    }
    // send is queued (the ble stack delays send to the start of the next tx window)
    while (! RFduinoBLE.send(buf, 20))
      ;  // all tx buffers in use (can't send - try again later)

    //if (! start)
      //start = millis();

    packet++;
    if (packet >= packets)
    {
      //int end = millis();
      //float secs = (end - start) / 1000.0;
      //int bps = ((packets * 20) * 8) / secs; 
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

void loopBLE(bool setup_flag)
{
  // this loop is activated when we enter the power conservation mode
  if(setup_flag == true) {
     RFduinoBLE.advertisementData = "h"; // shouldnt be more than 10 characters long
     RFduinoBLE.deviceName = ble_name;  //  name of your RFduino. Will appear when other BLE enabled devices search for it
     RFduinoBLE.begin(); // begin
     packets = 48 * m.pagecounter;
  }
}

