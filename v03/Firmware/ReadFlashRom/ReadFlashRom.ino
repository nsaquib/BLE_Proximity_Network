#include <PrNetRomManager.h>

#define PAGES_TO_READ 15

void setup() {
  PrNetRomManager m;
  Serial.begin(9600);

  // Iterate over pages
  for (int i = STORAGE_FLASH_PAGE; i > STORAGE_FLASH_PAGE - PAGES_TO_READ; i--) {
    m.printPage(i);
    //m.erasePage(i);
  }
}

void loop() {

}
