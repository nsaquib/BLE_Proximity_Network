#include <SimbleeCOM.h>
#include <Time.h>

#define START_HOUR 16
#define START_MINUTE 0
#define END_HOUR 23
#define END_MINUTE 59

char payload[] = { 0 };
int packets;

Time timer;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(5);
  Serial.println("Waiting for parameters...");
  while (Serial.available() == 0) {
    delay(5);
  }
  Serial.println(millis());
  timer.initialMillis = millis();
  char parameters[16];
  Serial.readBytes(parameters, 16);
  Serial.println(parameters);
  timer.setInitialTime(
    (parameters[0] - '0') * 10 + (parameters[1] - '0'),                                   // Month
    (parameters[2] - '0') * 10 + (parameters[3] - '0'),                                   // Date
    (parameters[4] - '0') * 10 + (parameters[5] - '0'),                                   // Year
    (parameters[6] - '0'),                                                                // Day
    (parameters[7] - '0') * 10 + (parameters[8] - '0'),                                   // Hour
    (parameters[9] - '0') * 10 + (parameters[10] - '0'),                                  // Minute
    (parameters[11] - '0') * 10 + (parameters[12] - '0'),                                 // Second
    (parameters[13] - '0') * 100 + (parameters[14] - '0') * 10 + (parameters[15] - '0')); // Millisecond
  Serial.println(millis());
  timer.displayDateTime();
  Serial.println("Received parameters.");
}

void loop() {
  if (timer.inDataCollectionPeriod(START_HOUR, START_MINUTE, END_HOUR, END_MINUTE)) {
    timer.displayDateTime();
    SimbleeCOM.begin();
    SimbleeCOM.send(payload, sizeof(payload));
    struct currentTime t = timer.getTime();
    delay(max(0, 1000 - ((t.seconds * 1000 + t.ms) % 1000)));
    SimbleeCOM.end();
    Simblee_ULPDelay(5000);
  } else {
    struct sleepTime sleepTime = timer.getTimeUntilStartTime(START_HOUR, START_MINUTE);
    int sleepTimeMillis = sleepTime.ms + (1000 * sleepTime.seconds) + (60000 * sleepTime.minutes) + (3600000 * sleepTime.hours) + (86400000 * sleepTime.days);
    Serial.println("Sleeping for " + String(sleepTime.days) + ":" + String(sleepTime.hours) + ":" + String(sleepTime.minutes) + ":" + String(sleepTime.seconds) + ":" + String(sleepTime.ms));
    Simblee_ULPDelay(sleepTimeMillis);
  }
}

void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi) {
  printf("%d ", rssi);
  printf("0x%08x ", esn);
  printf("\n");
}
