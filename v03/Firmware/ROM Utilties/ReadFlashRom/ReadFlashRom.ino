#include <PrNetRomManager.h>

#define PAGES_TO_READ 5
#define BAUD_RATE 9600
//#define MY_FLASH_PAGE  135
//#define MY_FLASH_VALUE  777

#define  str(x)   xstr(x)
#define  xstr(x)  #x

void setup() {
  PrNetRomManager m;
  Serial.begin(BAUD_RATE);
  // Iterate over pages
  for (int i = STORAGE_FLASH_PAGE; i > STORAGE_FLASH_PAGE - PAGES_TO_READ; i--) {
    m.printPage(i);
  }

  // a flash page is 1K in length, so page 251 starts at address 251 * 1024 = 257024 = 3EC00 hex
//  uint32_t *p = ADDRESS_OF_PAGE(MY_FLASH_PAGE);
//  int rc;
//   
//  Serial.print("The value stored in flash page " str(MY_FLASH_PAGE) " is: ");
//  Serial.println(*p, DEC);
//
//  Serial.println("Writing...");
//
//  Serial.print("Attempting to store " str(MY_FLASH_VALUE) " in flash page " str(MY_FLASH_PAGE) ": ");
//  rc = flashWrite(p, MY_FLASH_VALUE);
//  if (rc == 0)
//    Serial.println("Success");
//
//  Serial.print("The value stored in flash page " str(MY_FLASH_PAGE) " is: ");
//  Serial.println(*p, DEC);
  
//  Serial.println("Reading ROM complete");
}

void loop() {}


