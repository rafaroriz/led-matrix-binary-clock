#include <RTClib.h>
#include <Wire.h>
#include <LedControl.h>
#include <IRremote.h>
#include <DHT.h>

// display setup
// pin 10 is DataIn
// pin 11 is CLK
// pin 12 is LOAD(CS)
LedControl lc = LedControl(22, 23, 24, 1);

// clock setup
// pin A4 is SDA
// pin A5 is SCL
RTC_DS1307 RTC;

// infra-red LED setup
IRsend irsend;

// pin A1 is photoresistor
#define LDR 1
int lightIndex;
bool displayOn = true;
bool lightLamp = true;
bool lowLight = false;

// thermistor setup
// pin A0 is thermistor
#define THERMISTOR 0
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
byte Tc;
byte Tf;
byte temp;

// DHT setup
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int humidityIndex;
bool humidityRead = false;

// buttons and switches
#define HOUR_BTN 4
#define MIN_BTN 5
#define ALARM_BTN 50
#define MODE_SWITCH_UP 51
#define MODE_SWITCH_DOWN 53
#define MAIN_BTN 12

enum LED_BARS { FRONT, TOP };

// front LED bar
#define FRONT_BAR_0 39
#define FRONT_BAR_1 38
#define FRONT_BAR_2 37
#define FRONT_BAR_3 36
#define FRONT_BAR_4 35
#define FRONT_BAR_5 34
#define FRONT_BAR_6 33
#define FRONT_BAR_7 32
#define FRONT_BAR_8 31
#define FRONT_BAR_9 30

// top LED bar
#define TOP_BAR_0 40
#define TOP_BAR_1 41
#define TOP_BAR_2 42
#define TOP_BAR_3 43
#define TOP_BAR_4 44
#define TOP_BAR_5 45
#define TOP_BAR_6 46
#define TOP_BAR_7 47
#define TOP_BAR_8 48
#define TOP_BAR_9 49

// button LED
#define BUTTON_LED 13

// alarm buzzer
#define BUZZER 10

// analog pin to randomize random seed
#define RANDOMIZER_PIN 15

// buttons and switches variables
bool alarmScreen = false;
bool mainPressed = false;
bool modeUp = false;
bool modeDown = false;
bool hourPressed = false;
bool minPressed = false;

// time configs
byte clkH;
byte clkM;
byte clkS;
byte dateD;
byte dateM;
bool toggleAdjust = false;
int preAdjustTimer;

// alarm configs
bool alarmSwitch = false;
bool alarmRang = false;
int timeToAlarm;
int randomTune;
byte bkpAlarmH = alarmH;
byte bkpAlarmM = alarmM;

// random tune icon
byte tuneR0 = B00000100;
byte tuneR1 = B00110100;
byte tuneR2 = B00001000;

// HP tune icon
byte tuneH0 = B00111100;
byte tuneH1 = B00001000;
byte tuneH2 = B00110000;

// mario tune icon
byte tuneM0 = B00111100;
byte tuneM1 = B00001100;
byte tuneM2 = B00111100;

// poke intro tune icon
byte tuneP0 = B00111100;
byte tuneP1 = B00010100;
byte tuneP2 = B00001000;

// skyrim tune icon
byte tuneS0 = B00101100;
byte tuneS1 = B00101100;
byte tuneS2 = B00110100;

// zelda tune icon
byte tuneZ0 = B00110100;
byte tuneZ1 = B00100100;
byte tuneZ2 = B00101100;

byte tuneIcon0;
byte tuneIcon1;
byte tuneIcon2;

bool clockStep = false;
