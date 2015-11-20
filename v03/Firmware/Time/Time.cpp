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
}

void Time::setInitialTime(int month, int date, int year, int day, int hours, int minutes, int seconds, int ms) {
  initialTime.month = month;
  initialTime.date = date;
  initialTime.year = year;
  initialTime.day = day;
  initialTime.hours = hours;
  initialTime.minutes = minutes;
  initialTime.seconds = seconds;
  initialTime.ms = ms;
}

void Time::updateTime() {
  if (isTimeSet) {
    unsigned long ms = millis() - initialMillis;
    int carryOver;
    // Calculate each time component and propagate carryOver linearly
    currentTime.ms = (initialTime.ms + ms) % 1000;
    carryOver = (initialTime.ms + ms) / 1000;
    currentTime.seconds = (initialTime.seconds + carryOver) % 60;
    carryOver = (initialTime.seconds + carryOver) / 60;
    currentTime.minutes = (initialTime.minutes + carryOver) % 60;
    carryOver = (initialTime.minutes + carryOver) / 60;
    currentTime.hours = (initialTime.hours + carryOver) % 24;
    carryOver = (initialTime.hours + carryOver) / 24;
    currentTime.day = (initialTime.day + carryOver) % 7;
    // If in February, check if leap year
    if (currentTime.month == 1) {
      if (currentTime.year % 4 != 0) {
        currentTime.date = (initialTime.date + carryOver) % 28;
        carryOver = (initialTime.date + carryOver) / 28;
      } else {
       currentTime.date = (initialTime.date + carryOver) % 29;
       carryOver = (initialTime.date + carryOver) / 29;
      }
    }
    // If in April, June, September, or November
    if (currentTime.month == 3 || currentTime.month == 5 || currentTime.month == 8 || currentTime.month == 10) {
      currentTime.date = (initialTime.date + carryOver) % 30;
      carryOver = (initialTime.date + carryOver) / 30;
    } else {
      currentTime.date = (initialTime.date + carryOver) % 31;
      carryOver = (initialTime.date + carryOver) / 31;
    }
    currentTime.month = (initialTime.month + carryOver) % 12;
    carryOver = (initialTime.month + carryOver) / 12;
    currentTime.year = (initialTime.year + carryOver) % 99;
  }
}

int Time::getTimeUntilStartTime(int startHour, int startMinute) {
  // Number of ms, seconds, minutes, and hours to sleep until
  int days = 0;
  int hours = 0;
  int minutes = 0;
  int seconds = 0;
  int ms = 0;
  updateTime();
  if (currentTime.hours == startHour) {
    if (currentTime.minutes < startMinute) {
      hours = 0;
    } else {
      hours = 23;
    }
  } else if (currentTime.hours < startHour) {
    if (currentTime.minutes <= startMinute) {
       hours = startHour - currentTime.hours;
    } else {
      hours = startHour - currentTime.hours - 1;
    }
  } else {
    if (currentTime.minutes <= startMinute) {
      hours = 24 - currentTime.hours + startHour;
    } else {
      hours = 24 - currentTime.hours + startHour - 1; 
    }
  }
  if (currentTime.seconds <= 0) {
    minutes = (60 - currentTime.minutes + startMinute) % 60;
  } else {
    minutes = (60 - currentTime.minutes + startMinute - 1) % 60;
  }
  if (currentTime.ms <= 0) {
    seconds = (60 - currentTime.seconds) % 60;
  } else {
    seconds = (60 - currentTime.seconds - 1) % 60;
  }
  ms = (1000 - currentTime.ms) % 1000;
  // Calculate time to startHour:startMinute by converting higher order time units to ms
  int delayTime = ms + (1000 * seconds) + (60000 * minutes) + (3600000 * hours);
  // If its Friday, sleep through Saturday and Sunday
  if (currentTime.day == 5) {
    // Add 48 hours to delayTime in milliseconds
    days += 2;
    delayTime += 172800000;
  }
  Serial.print("Sleeping for ");
  Serial.print(days);
  Serial.print(":");
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
  updateTime();
  if (!isTimeSet) {
    return false;
  }
  // Collect data while within range
  if ((currentTime.hours > startHour) && (currentTime.hours < endHour)) {
    return true;
  // If start and end hour if the same, check minute on both endpoints
  } else if ((startHour == endHour) && ((currentTime.minutes < startMinute) || (currentTime.minutes >= endMinute))) {
    return false;
  // Inclusive on startMinute and exclusive on endMinute
  } else if (((currentTime.hours == startHour) && (currentTime.minutes >= startMinute))
            || ((currentTime.hours == endHour) && (currentTime.minutes < endMinute))) {
    return true;
  // Otherwise, do not collect data
  } else {
    return false;
  }
}

void Time::displayDate() {
  updateTime();
  Serial.print(currentTime.month);
  Serial.print("/");
  Serial.print(currentTime.date);
  Serial.print("/");
  Serial.print(currentTime.year);
  Serial.print(" ");
  Serial.print(currentTime.day);
  Serial.print(" ");
}

void Time::displayTime() {
  updateTime();
  Serial.print(currentTime.hours);
  Serial.print(":");
  Serial.print(currentTime.minutes);
  Serial.print(":");
  Serial.print(currentTime.seconds);
  Serial.print(":");
  Serial.println(currentTime.ms);
}

void Time::displayDateTime() {
  displayDate();
  displayTime();
}
