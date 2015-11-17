/*
 * Time.cpp - Library for maintaining discrete date and time representation for PrNet nodes.
 * Created by Dwyane George, November 16, 2015
 * Social Computing Group, MIT Media Lab
 */

#include "Arduino.h"
#include "Time.h"

Time::Time() {}

void Time::delayTime(int ms) {
  delay(ms);
  updateTime(ms);
}

void Time::setTime(int hours, int minutes, int seconds, int ms) {
  timer.hours = hours;
  timer.minutes = minutes;
  timer.seconds = seconds;
  timer.ms = ms;
}

void Time::setDate(int month, int day, int year, int weekday) {
  date.month = month;
  date.day = day;
  date.year = year;
  date.weekday = weekday;
}

void Time::updateTime(int ms) {
  // Update time only if its been set
  if (isTimeSet) {
    int carryOver;
    // Update each time component and propagate carryOver linearly
    timer.ms += ms;
    carryOver = floor(timer.ms/1000);
    if (timer.ms > 999) {
      timer.ms = timer.ms % 1000;
    }
    timer.seconds += carryOver;
    carryOver = floor(timer.seconds/60);
    if (timer.seconds > 59) {
      timer.seconds = timer.seconds % 60;
    }
    timer.minutes += carryOver;
    carryOver = floor(timer.minutes/60);
    if (timer.minutes > 59) {
      timer.minutes = timer.minutes % 60;
    }
    timer.hours += carryOver;
    carryOver = floor(timer.hours/24);
    if (timer.hours > 23) {
      timer.hours = timer.hours % 24;
    }
    date.day += carryOver;
    carryOver = floor(date.day/30);
    if (date.day > 30) {
      date.day = date.day % 30;
    }
    date.month += carryOver;
    carryOver = floor(date.month/12);
    if (date.month > 12) {
      date.month = date.month % 12;
    }
    date.year += carryOver;
    if (date.year > 99) {
      date.year = date.year % 99;
    }
  }
}

int Time::getTimeUntilStartTime(int startHour, int startMinute) {
  // Number of ms, seconds, minutes, and hours to sleep until
  int hours;
  int minutes;
  int seconds;
  int ms;
  if (timer.hours == startHour) {
    if (timer.minutes < startMinute) {
      hours = 0;
    } else {
      hours = 23;
    }
  } else if (timer.hours < startHour) {
    if (timer.minutes < startMinute) {
       hours = startHour - timer.hours;
    } else {
      hours = startHour - timer.hours - 1;
    }
  } else {
    if (timer.minutes < startMinute) {
      hours = 24 - timer.hours + startHour;
    } else {
      hours = 24 - timer.hours + startHour - 1; 
    }
  }
  if (timer.seconds <= 0) {
    minutes = (60 - timer.minutes + startMinute) % 60;
  } else {
    minutes = (60 - timer.minutes + startMinute - 1) % 60;
  }
  if (timer.ms <= 0) {
    seconds = (60 - timer.seconds) % 60;
  } else {
    seconds = (60 - timer.seconds - 1) % 60;
  }
  ms = (1000 - timer.ms) % 1000;
  // Calculate time to startHour:startMinute by converting higher order time units to ms
  int delayTime = ms + (1000 * seconds) + (60000 * minutes) + (3600000 * hours);
  // If its Friday, sleep through Saturday and Sunday
  if (date.day == 5) {
    // Add 48 hours to delayTime in milliseconds
    delayTime += 172800000;
  }
  Serial.print("Sleeping for ");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":");
  Serial.print(seconds);
  Serial.print(":");
  Serial.println(ms);
  return delayTime;
}

boolean Time::inDataCollectionPeriod(int startHour, int startMinute, int endHour, int endMinute) {
  if (!isTimeSet) {
    return false;
  }
  // Collect data while within range
  if ((timer.hours > startHour) && (timer.hours < endHour)) {
    return true;
  // Inclusive on startMinute and exclusive on endMinute
  } else if (((timer.hours == startHour) && (timer.minutes >= startMinute))
            || ((timer.hours == endHour) && (timer.minutes < endMinute))) {
    return true;
  // Otherwise, do not collect data
  } else {
    return false;
  }
}

void Time::displayDate() {
  Serial.print(date.month);
  Serial.print("/");
  Serial.print(date.day);
  Serial.print("/");
  Serial.print(date.year);
  Serial.print(" ");
}

void Time::displayTime() {
  Serial.print(timer.hours);
  Serial.print(":");
  Serial.print(timer.minutes);
  Serial.print(":");
  Serial.print(timer.seconds);
  Serial.print(":");
  Serial.println(timer.ms);
}

void Time::displayDateTime() {
  displayDate();
  displayTime();
}
