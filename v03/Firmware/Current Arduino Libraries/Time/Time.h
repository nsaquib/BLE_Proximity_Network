/*
 * Time.h - Library for maintaining discrete date and time representation for PrNet nodes.
 * Created by Dwyane George, November 16, 2015
 * Social Computing Group, MIT Media Lab
 */

#ifndef Time_h
#define Time_h

#include "Arduino.h"

// Initial time structure
struct initialTime {
  int month = 0;
  int date = 0;
  int year = 0;
  int day = 0;
  int hours = 0;
  int minutes = 0;
  int seconds = 0;
};

// Time structure
struct currentTime {
  int month = 0;
  int date = 0;
  int year = 0;
  int day = 0;
  int hours = 0;
  int minutes = 0;
  int seconds = 0;
};

// Sleep time structure
struct sleepTime {
  int days = 0;
  int hours = 0;
  int minutes = 0;
  int seconds = 0;
};

class Time {
  public:
    struct initialTime initialTime;
    struct currentTime currentTime;
    struct sleepTime sleepTime;
    volatile unsigned long secondsElapsed;
    bool isTimeSet;
    Time();
    void setInitialTime(int month, int date, int year, int day, int hours, int minutes, int seconds);
    struct currentTime getTime();
    void updateTime();
    struct sleepTime getTimeUntilStartTime(int startHour, int startMinute);
    bool inDataCollectionPeriod(int startHour, int startMinute, int endHour, int endMinute);
    void displayDate();
    void displayTime();
    void displayDateTime();
};

#endif