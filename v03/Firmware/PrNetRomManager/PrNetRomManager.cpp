/*
  PrNetRomManager.cpp - Library for managing data in the flash rom of PrNet nodes.
  Created by Nazmus Saquib, October 7, 2015
  Social Computing Group, MIT Media Lab.
*/

#include "Arduino.h"
#include "PrNetRomManager.h"

PrNetRomManager::PrNetRomManager() {
  pagecounter = 0;
}

void PrNetRomManager::printPage(int page) {
  Serial.println(page);
  data *p = (data*)ADDRESS_OF_PAGE(page);
  for(int i = 0; i < MAX_ROWS; i++) {
    printf("%d\n", p->data[i]);
  }
}

void PrNetRomManager::loadPage(int page) {
  data *p = (data*)ADDRESS_OF_PAGE(page);
  for(int i = 0; i < MAX_ROWS; i++) {
    table.data[i] = p->data[i];
  }
}


int PrNetRomManager::erasePage(int page) {
  int rc = flashPageErase(page);
  if (rc == 0) {
    return 0;
  } else {
    return rc;
  }
}

int PrNetRomManager::writePage(int page, struct data values) {
  data *p = (data*) ADDRESS_OF_PAGE(page);
  int rc = flashWriteBlock(p, &values, sizeof(values));
  if( rc == 0) {
    pagecounter++;
    return 0;
  } else {
    return rc;
  }
}

int PrNetRomManager::writePartialPage(int page, struct data values) {
  data *p = (data*) ADDRESS_OF_PAGE(page);
  int rc = flashWriteBlock(p, &values, sizeof(values));
  if( rc == 0) {
    return 0;
  } else {
    return rc;
  }
}

int PrNetRomManager::updateConfig(struct prnet_config c) {
  data *p = (data*) ADDRESS_OF_PAGE(SETTINGS_FLASH_PAGE);
  return flashWriteBlock(p, &c, sizeof(c));
}

struct prnet_config PrNetRomManager::loadConfig() {
  prnet_config *p = (prnet_config*)ADDRESS_OF_PAGE(SETTINGS_FLASH_PAGE);
  struct prnet_config a;
  a._curflashpage = p->_curflashpage;
  a._deviceID = p->_deviceID;
  a._pagecounter = p->_pagecounter;
  return a;
}