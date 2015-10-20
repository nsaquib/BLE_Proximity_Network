#include <PrNetRomManager.h>

void setup() {
  // put your setup code here, to run once:
  PrNetRomManager m;
  Serial.begin(9600);

  // iterate over pages
  for(int i = STORAGE_FLASH_PAGE; i > STORAGE_FLASH_PAGE - 70; i--)
  {
    m.printPage(i);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
