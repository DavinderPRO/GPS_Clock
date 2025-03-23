#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <TM1637Display.h>

// === Pin Configuration ===
#define GPS_RX 4
#define GPS_TX 3
#define CLK 5
#define DIO 6

// === Debug Print Toggle ===
#define ENABLE_DEBUG 0
#if ENABLE_DEBUG
  #define DBG_PRINT(x) Serial.print(x)
  #define DBG_PRINTLN(x) Serial.println(x)
#else
  #define DBG_PRINT(x)
  #define DBG_PRINTLN(x)
#endif

TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX, GPS_TX);
TM1637Display display(CLK, DIO);

// === Timing Variables ===
unsigned long lastTimeUpdate = 0;
unsigned long lastColonBlink = 0;
unsigned long lastDateDisplay = 0;
unsigned long lastStatusPrint = 0;

bool colonState = true;
bool timeTrusted = false;
bool showDateNow = false;

void setup() {
  #if ENABLE_DEBUG
    Serial.begin(9600);
  #endif
  ss.begin(9600);
  display.setBrightness(0x0f);
  display.showNumberDec(8888); // Startup animation
  delay(1000);
}

void loop() {
  while (ss.available()) {
    gps.encode(ss.read());
  }

  unsigned long now = millis();

  // Wait for GPS fix and reliable time
  if (!timeTrusted) {
    if (gps.time.isValid() && gps.satellites.value() > 3 && gps.time.age() < 2000) {
      timeTrusted = true;
      DBG_PRINTLN("GPS time is now trusted.");
    }

    // Print GPS status every 2 seconds while waiting
    if (now - lastStatusPrint >= 2000) {
      DBG_PRINT("Satellites: ");
      DBG_PRINT(gps.satellites.value());
      DBG_PRINT(" | Time age: ");
      DBG_PRINT(gps.time.age());
      DBG_PRINTLN(" ms");
      lastStatusPrint = now;
    }

    // Show waiting animation
    display.showNumberDec(8888);
    return;
  }

  // === Blink colon every 5 seconds ===
  if (now - lastColonBlink >= 5000) {
    colonState = !colonState;
    lastColonBlink = now;
  }

  // === Show date every 2 minutes ===
  if (now - lastDateDisplay >= 120000) {
    showDateNow = true;
    lastDateDisplay = now;
  }

  // === Update time every second ===
  if (now - lastTimeUpdate >= 1000) {
    lastTimeUpdate = now;

    if (showDateNow) {
      displayDate();
      showDateNow = false;
    } else {
      displayTime();
    }
  }
}

void displayTime() {
  int hour = gps.time.hour();
  int minute = gps.time.minute();

  // Timezone offset (IST +5:30)
  hour += 5;
  minute += 30;
  if (minute >= 60) {
    minute -= 60;
    hour++;
  }
  if (hour >= 24) {
    hour -= 24;
  }

  uint8_t colonFlag = colonState ? 0b01000000 : 0x00;
  display.showNumberDecEx(hour * 100 + minute, colonFlag, true);

  DBG_PRINT("Time: ");
  DBG_PRINT(hour);
  DBG_PRINT(":");
  DBG_PRINTLN(minute);
}

void displayDate() {
  int day = gps.date.day();
  int month = gps.date.month();

  display.showNumberDecEx(day * 100 + month, 0b01000000, true); // Show DD-MM
  DBG_PRINT("Date: ");
  DBG_PRINT(day);
  DBG_PRINT("-");
  DBG_PRINTLN(month);
  delay(3000);
}
