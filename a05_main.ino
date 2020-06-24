void setup() {
  // sets led-matrix brightness 0-15
  lc.setIntensity(0, 0);

  // starts RTC
  Wire.begin();
  RTC.begin();

  // starts DHT
  dht.begin();

  // pins configuration
  pinMode(SIDE_BTN_LED, OUTPUT);

  pinMode(FRONT_BAR_0, OUTPUT);
  pinMode(FRONT_BAR_1, OUTPUT);
  pinMode(FRONT_BAR_2, OUTPUT);
  pinMode(FRONT_BAR_3, OUTPUT);
  pinMode(FRONT_BAR_4, OUTPUT);
  pinMode(FRONT_BAR_5, OUTPUT);
  pinMode(FRONT_BAR_6, OUTPUT);
  pinMode(FRONT_BAR_7, OUTPUT);
  pinMode(FRONT_BAR_8, OUTPUT);
  pinMode(FRONT_BAR_9, OUTPUT);

  pinMode(TOP_BAR_0, OUTPUT);
  pinMode(TOP_BAR_1, OUTPUT);
  pinMode(TOP_BAR_2, OUTPUT);
  pinMode(TOP_BAR_3, OUTPUT);
  pinMode(TOP_BAR_4, OUTPUT);
  pinMode(TOP_BAR_5, OUTPUT);
  pinMode(TOP_BAR_6, OUTPUT);
  pinMode(TOP_BAR_7, OUTPUT);
  pinMode(TOP_BAR_8, OUTPUT);
  pinMode(TOP_BAR_9, OUTPUT);

  pinMode(HOUR_BTN, INPUT_PULLUP);
  pinMode(MIN_BTN, INPUT_PULLUP);
  pinMode(ALARM_BTN, INPUT_PULLUP);
  pinMode(MODE_SWITCH_UP, INPUT_PULLUP);
  pinMode(MODE_SWITCH_DOWN, INPUT_PULLUP);
  pinMode(SIDE_BTN, INPUT_PULLUP);

  // randomizes random seed
  randomSeed(analogRead(RANDOMIZER_PIN));

  // turns lamp off if the clock restarts in the middle of the night
  // this means there was a blackout and when power comes back the lamp will probably turn itself on
  DateTime now = RTC.now();
  clkH = now.hour();
  if (clkH >= 0 && clkH <= 6 && getLightIndex() >= LIGHT_THRESHOLD) {
    for (int i = 0; i < 3; i++) {
      irsend.sendNEC(0xFF609F, 32);
      delay(BASE_DELAY / 5);
    }
  }

  // initializes serial comm
  //Serial.begin(9600);

  // sets the RTC to the date & time this sketch was compiled
  //RTC.adjust(DateTime(__DATE__, __TIME__));

  // sets the RTC to the date & time defined below
  //RTC.adjust(DateTime(2019, 11, 17, 11, 59, 40));
}

void loop() {

  // gets time from RTC and sets date and time variables
  DateTime now = RTC.now();
  clkH = now.hour();
  clkM = now.minute();
  clkS = now.second();
  dateD = now.day();
  dateM = now.month();

  // reads buttons states
  hourPressed = digitalRead(HOUR_BTN);
  minPressed = digitalRead(MIN_BTN);
  alarmSwitch = digitalRead(ALARM_BTN);
  modeUp = digitalRead(MODE_SWITCH_UP);
  modeDown = digitalRead(MODE_SWITCH_DOWN);
  sidePressed = digitalRead(SIDE_BTN);

  // reads LDR value
  lightIndex = getLightIndex();

  // go to alarm screen when mode switch is up
  if (!modeUp) {
    alarmScreen = true;
    toggleAdjust = false;
  } else {
    alarmScreen = false;
  }

  // turns display and LEDs off when mode switch is down
  if (!modeDown && !toggleAdjust) {
    displayOn = false;
  } else {
    displayOn = true;
  }

  // set lowLight flag if lightIndex is below threshold
  if (lightIndex >= LIGHT_THRESHOLD) {
    lowLight = false;
  } else {
    lowLight = true;
  }

  // turns display and LEDs on or off according to lowLight flag
  if (!toggleAdjust) {
    if ((displayOn && !lowLight) || alarmScreen) {
      lc.shutdown(0, false);
    } else {
      lc.shutdown(0, true);
      digitalWrite(SIDE_BTN_LED, LOW);
      if (alarmSwitch || !displayOn) {
        setLEDBar(0, setBarInt(0));
        setLEDBar(1, setBarInt(0));
      }
    }
  } else {
    lc.shutdown(0, false);
  }

  // gets temperature
  temp = getTemp();

  // reads humidity every two seconds
  if (clkS % 2 == 0 && !humidityRead) {
    humidityIndex = map(dht.readHumidity(), 20, 90, 1, 10);
    humidityRead = true;
  } else if (clkS % 2 != 0) {
    humidityRead = false;
  }

  // sets front LED bar according to sensor readings
  if (displayOn && !lowLight) {
    if (humidityIndex == 1 && clockStep) {
      setLEDBar(FRONT, setBarInt(0));
    } else {
      setLEDBar(FRONT, setBarInt(humidityIndex));
    }
  }

  // sets top LED bar according to light index if alarm is not set
  if (displayOn && !lowLight && !alarmScreen && alarmSwitch) {
    setLEDBar(TOP, setBarInt(lightIndex));
  }

  // calculates time left before alarm
  if (alarmScreen || !alarmSwitch) {
    if (clkH > alarmH) {
      timeToAlarm = 24 - (clkH - alarmH);
    } else if (clkH < alarmH) {
      timeToAlarm = alarmH - clkH;
    } else {
      timeToAlarm = 0;
    }
    if (clkM > alarmM) {
      timeToAlarm--;
      if (timeToAlarm < 0) {
        timeToAlarm = 23;
      }
    }
  }

  // sets top LED bar according to time left for alarm if alarm is set
  if (displayOn && !alarmSwitch) {
    if (timeToAlarm == 0 & clockStep) {
      setLEDBar(TOP, setBarInt(1));
    } else if (timeToAlarm <= 10) {
      setLEDBar(TOP, setBarInt(timeToAlarm));
    } else {
      if (clockStep) {
        setLEDBar(TOP, setBarInt(10));
      } else {
        setLEDBar(TOP, setBarInt(0));
      }
    }
    // sets front LED bar according to time left for alarm if lowLight and alarm is set
    if (lowLight) {
      if (timeToAlarm == 0 & clockStep) {
        setLEDBar(FRONT, setBarInt(1));
      } else if (timeToAlarm <= 10) {
        setLEDBar(FRONT, setBarInt(timeToAlarm));
      } else {
        if (clockStep) {
          setLEDBar(FRONT, setBarInt(10));
        } else {
          setLEDBar(FRONT, setBarInt(0));
        }
      }
    }
  }

  // blinks top LED bar while on alarm screen if alarm is not yet armed
  if (displayOn && alarmScreen && alarmSwitch) {
    if (clockStep) {
      if (timeToAlarm == 0) {
        setLEDBar(TOP, setBarInt(1));
      } else if (timeToAlarm > 10) {
        setLEDBar(TOP, setBarInt(10));
      } else {
        setLEDBar(TOP, setBarInt(timeToAlarm));
      }
    } else {
      setLEDBar(TOP, setBarInt(0));
    }
  }

  // sets alarm tune icon to be displayed in alarm screen
  switch (alarmTune) {
    case 0:
      tuneIcon0 = tuneR0;
      tuneIcon1 = tuneR1;
      tuneIcon2 = tuneR2;
      break;
    case 1:
      tuneIcon0 = tuneH0;
      tuneIcon1 = tuneH1;
      tuneIcon2 = tuneH2;
      break;
    case 2:
      tuneIcon0 = tuneM0;
      tuneIcon1 = tuneM1;
      tuneIcon2 = tuneM2;
      break;
    case 3:
      tuneIcon0 = tuneP0;
      tuneIcon1 = tuneP1;
      tuneIcon2 = tuneP2;
      break;
    case 4:
      tuneIcon0 = tuneS0;
      tuneIcon1 = tuneS1;
      tuneIcon2 = tuneS2;
      break;
    case 5:
      tuneIcon0 = tuneZ0;
      tuneIcon1 = tuneZ1;
      tuneIcon2 = tuneZ2;
      break;
  }

  // blinks alarm time if alarm is armed and main button is pressed when display is off
  if (!alarmScreen && !alarmSwitch && !sidePressed && (!displayOn || lowLight)) {
    for (int i = 0; i < 3; i++) {
      lc.shutdown(0, false);
      digitalWrite(SIDE_BTN_LED, HIGH);
      lc.setRow(0, 0, reverse(alarmH) >> 1);
      lc.setRow(0, 1, reverse(alarmM) >> 1);
      lc.setRow(0, 2, B0);
      lc.setRow(0, 3, B01111110);
      lc.setRow(0, 4, B0);
      lc.setRow(0, 5, tuneIcon0);
      lc.setRow(0, 6, tuneIcon1);
      lc.setRow(0, 7, tuneIcon2);
      delay(BASE_DELAY * 5);
      lc.shutdown(0, true);
      digitalWrite(SIDE_BTN_LED, LOW);
      delay(BASE_DELAY);
    }
  }

  // sends info to display
  if (!alarmScreen) {
    // main screen
    lc.setRow(0, 0, reverse(clkH) >> 1);
    lc.setRow(0, 1, reverse(clkM) >> 1);
    lc.setRow(0, 2, reverse(clkS) >> 1);
    lc.setRow(0, 3, B0);
    lc.setRow(0, 4, B0);
    lc.setRow(0, 5, reverse(dateD) >> 1);
    lc.setRow(0, 6, reverse(dateM) >> 1);
    lc.setRow(0, 7, reverse(temp) >> 1);
  } else {
    // alarm screen
    lc.setRow(0, 0, reverse(alarmH) >> 1);
    lc.setRow(0, 1, reverse(alarmM) >> 1);
    lc.setRow(0, 2, B0);
    lc.setRow(0, 3, B01111110);
    lc.setRow(0, 4, B0);
    lc.setRow(0, 5, tuneIcon0);
    lc.setRow(0, 6, tuneIcon1);
    lc.setRow(0, 7, tuneIcon2);
  }

  // compensates for RTC rushing ~8 secs per day
  if (clkH == 5 && clkM == 58 && clkS == 59) {
    delay(7950);
    RTC.adjust(DateTime(now.year(), now.month(), now.day(), 5, 59, 0));
  }

  // waits for time adjust confirmation when main button is pressed
  if (displayOn && !lowLight && alarmSwitch && !alarmScreen && !toggleAdjust && !sidePressed) {
    delay(BASE_DELAY);
    toggleAdjust = true;
    preAdjustTimer = clkS + 5;
    if (preAdjustTimer >= 60) {
      preAdjustTimer -= 60;
    }
  } // blinks LED while waiting
  if (toggleAdjust && preAdjustTimer != clkS && clockStep) {
    digitalWrite(SIDE_BTN_LED, LOW);
  } else if (toggleAdjust && preAdjustTimer != clkS && !clockStep) {
    digitalWrite(SIDE_BTN_LED, HIGH);
  } else if (toggleAdjust && preAdjustTimer == clkS) {
    toggleAdjust = false;
  }

  // adjusts time manually
  if (toggleAdjust && !modeDown) {
    beep();

    while (!modeDown) {
      DateTime now = RTC.now();
      byte clkH = now.hour();
      byte clkM = now.minute();
      byte clkS = now.second();

      lc.clearDisplay(0);
      lc.setRow(0, 5, reverse(dateD) >> 1);
      lc.setRow(0, 6, reverse(dateM) >> 1);
      lc.setRow(0, 7, reverse(temp) >> 1);
      digitalWrite(SIDE_BTN_LED, LOW);
      delay(100);
      lc.setRow(0, 0, reverse(clkH) >> 1);
      lc.setRow(0, 1, reverse(clkM) >> 1);
      digitalWrite(SIDE_BTN_LED, HIGH);
      delay(400);

      modeDown = digitalRead(MODE_SWITCH_DOWN);
      hourPressed = digitalRead(HOUR_BTN);
      minPressed = digitalRead(MIN_BTN);

      if (!hourPressed) {
        clkH = clkH + 1;
        if (clkH >= 24) {
          clkH = 0;
        }
      }
      if (!minPressed) {
        clkM = clkM + 1;
        if (clkM >= 60) {
          clkM = 0;
        }
      }
      RTC.adjust(DateTime(now.year(), now.month(), now.day(), clkH, clkM, 0));
    }
    toggleAdjust = false;
    beep();
  }

  // sets alarm
  if (alarmScreen && !hourPressed) {
    alarmH = alarmH + 1;
    if (alarmH >= 24) {
      alarmH = 0;
    }
    bkpAlarmH = alarmH;
    delay(BASE_DELAY);
  }
  if (alarmScreen && !minPressed) {
    alarmM = alarmM + 5;
    if (alarmM >= 60) {
      alarmM = 0;
    }
    delay(BASE_DELAY);
    bkpAlarmM = alarmM;
  }

  // sets alarm tune
  if (alarmScreen && !sidePressed) {
    alarmTune++;
    if (alarmTune > 5) {
      alarmTune = 0;
    }
    delay(BASE_DELAY);
  }

  // toggles light lamp function and turns side button LED on or off accordingly
  if (displayOn && !alarmScreen && !alarmSwitch && !sidePressed) {
    lightLamp = !lightLamp;
    delay(BASE_DELAY);
  }
  if (!toggleAdjust) {
    if ((displayOn && !lowLight && !alarmSwitch && lightLamp) ||
      (alarmScreen && !alarmSwitch && lightLamp)) {
      digitalWrite(SIDE_BTN_LED, HIGH);
    } else {
      digitalWrite(SIDE_BTN_LED, LOW);
    }
  }
  // blinks side button LED if on alarm screen but alarm is not armed yet
  if (alarmScreen && alarmSwitch && lightLamp) {
    if (clockStep) {
      digitalWrite(SIDE_BTN_LED, HIGH);
    } else {
      digitalWrite(SIDE_BTN_LED, LOW);
    }
  }

  // turns lamp on before alarm rings if room is dark
  if (lightLamp && !alarmSwitch && lightIndex < ALARM_LIGHT_THRESHOLD &&
      ((alarmH == clkH && alarmM - PRE_ALARM_TIME == clkM && clkS == 0) ||
       (PRE_ALARM_TIME > 0 && alarmH - 1 == clkH && alarmM == 0 && clkM == 60 - PRE_ALARM_TIME &&  clkS == 0)
      )) {
    for (int i = 0; i < 5; i++) {
      lampOn();
      if (getLightIndex() >= ALARM_LIGHT_THRESHOLD) {
        break;
      }
    }
  }

  // plays alarm
  if (!alarmSwitch && clkH == alarmH && clkM == alarmM) {
    // keeps display on while alarm is active
    lc.shutdown(0, false);
    
    digitalWrite(SIDE_BTN_LED, HIGH);
    playAlarm(alarmTune);

    // increments alarm time for snooze
    alarmM = alarmM + 5;
    if (alarmM >= 60) {
      alarmM = 0;
      alarmH = alarmH + 1;
    }
    alarmRang = true;
  }

  // resets alarm time back to where it was if it has snoozed
  if (clkH == alarmH + 1 && clkM == alarmM && alarmRang) {
    alarmH = bkpAlarmH;
    alarmM = bkpAlarmM;
    alarmRang = false;
  }

  // alternates clockStep each main loop cycle
  clockStep = !clockStep;

  delay(BASE_DELAY);
}
