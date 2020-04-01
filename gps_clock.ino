
#include <TM1637.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#define SRX 4
#define STX 3
#define CLK 5
#define DIO 6

TM1637 tm1637(CLK, DIO);
SoftwareSerial ss(SRX, STX);
TinyGPS gps;

static void smartdelay(unsigned long ms);
static void get_date(TinyGPS &gps);
static void Print_LED(uint8_t hour, uint8_t minute);
static boolean Pointonff = true;
// static void print_int(unsigned long val, unsigned long invalid, int len);

void setup()
{
  /// Serial.begin(9600);
  ss.begin(9600);
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);
  Print_LED(0, 0);
}

void loop()
{
  get_date(gps);
  smartdelay(2000);
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());

  } while (millis() - start < ms);
}

static void get_date(TinyGPS &gps)
{
  int year;
  uint8_t month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);

  //  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);

  if (age == TinyGPS::GPS_INVALID_AGE || age > 20000)
  {
    //  Serial.print("Time: 88:88 ");
    Print_LED(88, 88);
  }
  else if (age < 20000)
  {
    hour += 5;
    minute += 30;
    if (minute > 59)
    {
      minute -= 60;
      hour += 1;
    }
    if (hour > 23)
    {
      hour -= 24;
    }

    char sz[32];
    sprintf(sz, "Time: %02d:%02d ", hour, minute);
    //  Serial.print(sz);
    Print_LED(hour, minute);
  }

  //  Serial.println();
}

static void Print_LED(uint8_t hour, uint8_t minute)
{

  uint8_t u8arry[4] = {8, 8, 8, 8};
  if (hour < 10)
  {
    u8arry[0] = 0;
    u8arry[1] = hour;
  }
  else
  {
    u8arry[0] = hour / 10;
    u8arry[1] = hour % 10;
  }

  if (minute < 10)
  {
    u8arry[2] = 0;
    u8arry[3] = minute;
  }
  else
  {
    u8arry[2] = minute / 10;
    u8arry[3] = minute % 10;
  }
  tm1637.display(0, u8arry[0]);
  tm1637.display(1, u8arry[1]);
  tm1637.display(2, u8arry[2]);
  tm1637.display(3, u8arry[3]);
  Pointonff != Pointonff;
  tm1637.point(Pointonff);
}
/*
static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "  Age:  %ld", val);

  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';

  Serial.print(sz);
  smartdelay(0);
}

*/