#include <PrNetRomManager.h>

#define PAGES_TO_READ 5
#define BAUD_RATE 250000

void setup() {
  PrNetRomManager m;
  Serial.begin(BAUD_RATE);

  // Iterate over pages
  for(int i = STORAGE_FLASH_PAGE; i > STORAGE_FLASH_PAGE - PAGES_TO_READ; i--) {
    m.erasePage(i);
    m.printPage(i);
  }
  Serial.println("Erasing ROM complete");
}

void loop() {}
