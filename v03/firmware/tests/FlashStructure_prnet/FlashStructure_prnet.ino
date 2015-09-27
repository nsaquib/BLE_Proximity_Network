/*
This sketch demonstrates how to read and write a structure
to flash memory on the RFduino.

This example is located under RFduinoNonBLE, but the same
code can be used with Bluetooth Low Energy 4 functionality.

Data logging test for prnet nodes..
*/

// select a flash page that isn't in use (see Memory.h for more info)
#define  MY_FLASH_PAGE  251

// we can hold 80 rows of 12 bytes data (t, id, rssi) to be under 1K page memory
#define lenrec 80

// double level of indirection required to get gcc
// to apply the stringizing operator correctly
#define  str(x)   xstr(x)
#define  xstr(x)  #x

struct data_t
{
  int t[lenrec] = {0};
  // the compiler will insert 3 bytes of padding here so id is aligned on a dword boundary
  int id[lenrec] = {0};
  int rsval[lenrec] = {0};
};

//struct data_t value = { '1', 2, 3.0 };

struct data_t value;

void dump_data( struct data_t *p )
{
  Serial.print("  a = ");
  Serial.println(p->t[lenrec-1]);
  Serial.print("  b = ");
  Serial.println(p->id[lenrec-1]);
  Serial.print("  c = ");
  Serial.println(p->rsval[lenrec-1]);  
}

void setup() {
  Serial.begin(38400);
  Serial.println("All output will appear on the serial monitor.");

  // a flash page is 1K in length, so page 251 starts at address 251 * 1024 = 257024 = 3EC00 hex
  data_t *p = (data_t*)ADDRESS_OF_PAGE(MY_FLASH_PAGE);
  int rc;

  Serial.println("The data stored in flash page " str(MY_FLASH_PAGE) " contains: ");
  dump_data(p);

  // fill up the struct with values
  for (int i = 0; i < lenrec; i++)
  {
    value.t[i] = value.id[i] = value.rsval[i] = i*2;
  }
  
  ///*
  Serial.print("Attempting to erase flash page " str(MY_FLASH_PAGE) ": ");
  rc = flashPageErase(PAGE_FROM_ADDRESS(p));
  if (rc == 0)
    Serial.println("Success");
  else if (rc == 1)
    Serial.println("Error - the flash page is reserved");
  else if (rc == 2)
    Serial.println("Error - the flash page is used by the sketch");

  Serial.println("The data stored in flash page " str(MY_FLASH_PAGE) " contains: ");
  dump_data(p);
  
  Serial.print("Attempting to write data to flash page " str(MY_FLASH_PAGE) ": ");
  rc = flashWriteBlock(p, &value, sizeof(value));
  if (rc == 0)
    Serial.println("Success");
  else if (rc == 1)
    Serial.println("Error - the flash page is reserved");
  else if (rc == 2)
    Serial.println("Error - the flash page is used by the sketch");

  Serial.println("The data stored in flash page " str(MY_FLASH_PAGE) " contains: ");
  dump_data(p);
  //*/
}

void loop() {
}

