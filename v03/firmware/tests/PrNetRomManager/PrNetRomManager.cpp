/*
  PrNetRomManager.cpp - Library for managing data in the flash rom of PrNet nodes.
  Created by Nazmus Saquib, October 7, 2015.
  Social Computing Group, MIT Media Lab.
*/

#include "Arduino.h"
#include "PrNetRomManager.h"

PrNetRomManager::PrNetRomManager()
{
  //loadConfig();
  //pagecounter = 0;
  //pinMode(pin, OUTPUT);
  //_pin = pin;
}

void PrNetRomManager::printPage(int page)
{
  data_t *p = (data_t*)ADDRESS_OF_PAGE(page);
  Serial.println(p->t[lenrec-1]);
}

int PrNetRomManager::erasePage(int page)
{
  return flashPageErase(page);
}

int PrNetRomManager::writePage(int page, struct data_t values)
{
  data_t *p = (data_t*)ADDRESS_OF_PAGE(page);
  int rc = flashWriteBlock(p, &values, sizeof(values));
  if( rc == 0)
  {
    pagecounter++;
    return 0;
  }
  else
  {
    return rc;
  }
}

int PrNetRomManager::updateConfig(struct prnet_config c)
{
  data_t *p = (data_t*)ADDRESS_OF_PAGE(SETTINGS_FLASH_PAGE);
  return flashWriteBlock(p, &c, sizeof(c));
}

struct prnet_config PrNetRomManager::loadConfig()
{
  prnet_config *p = (prnet_config*)ADDRESS_OF_PAGE(SETTINGS_FLASH_PAGE);
  struct prnet_config a;
  a._curflashpage = p->_curflashpage;
  a._deviceID = p->_deviceID;
  a._pagecounter = p->_pagecounter;
  return a;
}

