#include <PrNetRomManager.h>

void setup() {
  PrNetRomManager m;
  Serial.begin(250000);

  // Iterate over pages
  for (int i = STORAGE_FLASH_PAGE; i > STORAGE_FLASH_PAGE - 60; i--) {
    m.printPage(i);
    //m.erasePage(i);
  }
}

void loop() {

}
