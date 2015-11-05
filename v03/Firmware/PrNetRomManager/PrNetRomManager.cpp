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
  pagecounter = 0;
}

void PrNetRomManager::printPage(int page)
{
  Serial.println(page);
  data_t *p = (data_t*)ADDRESS_OF_PAGE(page);
  for(int i = 0; i < lenrec; i++)
  {
    printf("%d,%d,%d\n",p->t[i],p->id[i],p->rsval[i]);
  }
}

void PrNetRomManager::loadPage(int page)
{
  //Serial.println(page);
  data_t *p = (data_t*)ADDRESS_OF_PAGE(page);
  for(int i = 0; i < lenrec; i++)
  {
    table.t[i] = p->t[i];
    table.id[i] = p->id[i];
    table.rsval[i] = p->rsval[i];
  }
}


int PrNetRomManager::erasePage(int page)
{
  int rc = flashPageErase(page);
  if (rc == 0)
  {
    //pagecounter--;
    return 0;
  }
  else
  {
    return rc;
  }
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

