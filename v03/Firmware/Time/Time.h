/*
 * Time.h - Library for maintaining discrete date and time representation for PrNet nodes.
 * Created by Dwyane George, November 16, 2015
 * Social Computing Group, MIT Media Lab
 */

#ifndef Time_h
#define Time_h

#include "Arduino.h"

// Time structure
struct timer {
  int hours = 0;
  int minutes = 0;
  int seconds = 0;
  int ms = 0;
};

// Date structure
struct date {
  int month = 0;
  int day = 0;
  int year = 0;
  int weekday = 0;
};

class Time {
  public:
    struct timer timer;
    struct date date;
    boolean isTimeSet;
    Time();
    void delayTime(int ms);
    void setTime(int hours, int minutes, int seconds, int milliseconds);
    void setDate(int month, int day, int year, int weekday);
    void updateTime(int ms);
    int getTimeUntilStartTime(int startHour, int startMinute);
    boolean inDataCollectionPeriod(int startHour, int startMinute, int endHour, int endMinute);
    void displayDate();
    void displayTime();
    void displayDateTime();
};

#endif