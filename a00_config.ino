#define LIGHT_THRESHOLD 1             // display will shutdown if ambient light goes below this threshold (0-10)
#define ALARM_LIGHT_THRESHOLD 5       // ambient light needs to be below this threshold to lamp turn on before alarm (0-10)
#define PRE_ALARM_TIME 2              // time to light lamp before alarm plays (minutes)
#define BASE_DELAY 200                // base delay time used to refresh display (milisecs)

byte alarmH = 7;                      // default alarm hour
byte alarmM = 0;                      // default alarm minute
byte alarmTune = 0;                   // default alarm tune
