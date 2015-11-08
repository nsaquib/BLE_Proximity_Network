#include <PrNetRomManager.h>

void setup() {
  // put your setup code here, to run once:
  PrNetRomManager m;
  Serial.begin(250000);

  // iterate over pages
  for(int i = STORAGE_FLASH_PAGE; i >= 0; i--)
  {
    m.erasePage(i);
    m.printPage(i);
  }
  Serial.println("Erase complete");
}

void loop() {
  // put your main code here, to run repeatedly:

}
