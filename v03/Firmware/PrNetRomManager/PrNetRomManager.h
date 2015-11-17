/*
  PrNetRomManager.h - Library for managing data in the flash rom of PrNet nodes.
  Created by Nazmus Saquib, October 7, 2015
  Social Computing Group, MIT Media Lab
*/

#ifndef PrNetRomManager_h
#define PrNetRomManager_h

#include "Arduino.h"

// Select a flash page that isn't in use (see Memory.h for more info)
#define SETTINGS_FLASH_PAGE  251
#define STORAGE_FLASH_PAGE 250
// We can hold 240 rows of 4 bytes data to be under 1K page memory
#define MAX_ROWS 240
// Double level of indirection required to get gcc to apply the stringizing operator correctly
#define  str(x)   xstr(x)
#define  xstr(x)  #x

struct data {
  unsigned int data[MAX_ROWS] = {0};
};

struct prnet_config {
  int _curflashpage = STORAGE_FLASH_PAGE;
  int _deviceID = 0;
  int _pagecounter = 0;
};

class PrNetRomManager {
  public:
    struct data table;
    struct prnet_config config;
    long pagecounter;
    long curflashpage;
    PrNetRomManager();
    void printPage(int page);
    void loadPage(int page);
    int erasePage(int page);
    int writePage(int page, struct data values);
    int updateConfig(struct prnet_config c);
    struct prnet_config loadConfig();
  private:
    int _pin;
};

#endif