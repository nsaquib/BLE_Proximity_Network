#include <PrNetRomManager.h>

void setup() {
  // put your setup code here, to run once:
  PrNetRomManager m;
  Serial.begin(9600);

  // iterate over pages
  for(int i = STORAGE_FLASH_PAGE; i > STORAGE_FLASH_PAGE - 30; i--)
  {
    m.printPage(i);
  }
  /*
  m.table.t[78] = 200; 
  m.table.t[79] = 100; 
  m.printPage(250);
  m.writePage(250, m.table);
  m.printPage(250);
  m.erasePage(250);
  m.printPage(250);
  */
}

void loop() {
  // put your main code here, to run repeatedly:

}
