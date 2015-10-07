/*
  PrNetRomManager.h - Library for managing data in the flash rom of PrNet nodes.
  Created by Nazmus Saquib, September 8, 2015.
  Social Computing Group, MIT Media Lab
*/

#ifndef PrNetRomManager_h
#define PrNetRomManager_h

#include "Arduino.h"

// select a flash page that isn't in use (see Memory.h for more info)
#define SETTINGS_FLASH_PAGE  251

#define STORAGE_FLASH_PAGE 250
// we can hold 80 rows of 12 bytes data (t, id, rssi) to be under 1K page memory
// or we can hold 125 rows of 8 bytes data (t + id, rssi) to be under 1K page memory
#define lenrec 80

// double level of indirection required to get gcc
// to apply the stringizing operator correctly
#define  str(x)   xstr(x)
#define  xstr(x)  #x

struct data_t
{
  int t[lenrec] = {0};
  int id[lenrec] = {0};
  int rsval[lenrec] = {0};
};

struct prnet_config
{
  int _curflashpage = STORAGE_FLASH_PAGE;
  int _deviceID = 0;
  int _pagecounter = 0;
};

//struct data_t values;

//long counter = 0;

//int curflashpage = STORAGE_FLASH_PAGE - hostcounter;

class PrNetRomManager
{
  public:
    struct data_t table;
    struct prnet_config config;
    //int t[lenrec] = {0};
    //int id[lenrec] = {0};
    //int rsval[lenrec] = {0};
    long pagecounter;
    long curflashpage;

    PrNetRomManager();
    void printPage(int page);
    int erasePage(int page);
    int writePage(int page, struct data_t values); 
    //bool isPageAvailable(int page);
    // update configuration page. write a different program to flash every prnet module
    // with the desired configuration.
    int updateConfig(struct prnet_config c);
    // load configuration from the settings page
    struct prnet_config loadConfig();
  private:
    int _pin;
};

#endif