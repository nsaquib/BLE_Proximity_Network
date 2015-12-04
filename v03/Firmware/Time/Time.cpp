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
  int carryOver;
  updateTime();
  ms = (1000 - currentTime.ms) % 1000;
  carryOver = (ms > 0) ? 1 : 0;
  seconds = (60 - currentTime.seconds - carryOver) % 60;
  carryOver = (seconds > 0 || ms > 0) ? 1 : 0;
  minutes = (60 - currentTime.minutes + startMinute - carryOver) % 60;
  carryOver = ((currentTime.minutes + carryOver) % 60 > startMinute || seconds > 0 || ms > 0) ? 1 : 0;
  if (currentTime.hours == startHour) {
    if (currentTime.minutes + carryOver < startMinute) {
      hours = 0;
    } else {
      hours = 23;
    }
  } else if (currentTime.hours < startHour) {
    if (currentTime.minutes + carryOver <= startMinute) {
       hours = startHour - currentTime.hours;
    } else {
      hours = startHour - currentTime.hours - 1;
    }
  } else {
    if (currentTime.minutes + carryOver <= startMinute) {
      hours = 24 - currentTime.hours + startHour;
    } else {
      hours = 24 - currentTime.hours + startHour - 1; 
    }
  }
  // If its Friday, sleep through Saturday and Sunday
  if (currentTime.day == 5) {
    days += 2;
  }
  // If its Saturday, sleep through Sunday
  if (currentTime.day == 6) {
    days += 1;
  }
  // Calculate time to startHour:startMinute by converting higher order time units to ms
  int delayTime = ms + (1000 * seconds) + (60000 * minutes) + (3600000 * hours) + (86400000 * days);
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
  // Inclusive on startMinute with check on seconds and milliseconds and exclusive on endMinute
  } else if (((currentTime.hours == startHour) && (currentTime.minutes > startMinute))
  			|| ((currentTime.hours == startHour) && (currentTime.minutes == startMinute) && currentTime.seconds == 0 && currentTime.ms == 0)
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
