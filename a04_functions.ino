byte getTemp() {
  Vo = analogRead(THERMISTOR);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  temp = (int)T - 273.15;                                         // temperature in celsius
  //temp = (int)((T - 273.15) * 1.8 + 32);                        // temperature in fahrenheit

  return temp;
}

byte reverse(byte in) {
  byte out = 0;
  for (int i = 0; i < 8; i++)
    out |= ((in >> i) & 1) << (7 - i);
  return out;
}

int setBarInt(byte bitIndex) {
  int LEDBar = 0;
  if (bitIndex != 0) {
    bitSet(LEDBar, bitIndex);
  }
  return LEDBar;
}

void setLEDBar(int bar, int intBit) {
  switch (bar) {
    case FRONT:
      digitalWrite(FRONT_BAR_0, bitRead(intBit, 1));
      digitalWrite(FRONT_BAR_1, bitRead(intBit, 2));
      digitalWrite(FRONT_BAR_2, bitRead(intBit, 3));
      digitalWrite(FRONT_BAR_3, bitRead(intBit, 4));
      digitalWrite(FRONT_BAR_4, bitRead(intBit, 5));
      digitalWrite(FRONT_BAR_5, bitRead(intBit, 6));
      digitalWrite(FRONT_BAR_6, bitRead(intBit, 7));
      digitalWrite(FRONT_BAR_7, bitRead(intBit, 8));
      digitalWrite(FRONT_BAR_8, bitRead(intBit, 9));
      digitalWrite(FRONT_BAR_9, bitRead(intBit, 10));
      break;
    case TOP:
      digitalWrite(TOP_BAR_0, bitRead(intBit, 1));
      digitalWrite(TOP_BAR_1, bitRead(intBit, 2));
      digitalWrite(TOP_BAR_2, bitRead(intBit, 3));
      digitalWrite(TOP_BAR_3, bitRead(intBit, 4));
      digitalWrite(TOP_BAR_4, bitRead(intBit, 5));
      digitalWrite(TOP_BAR_5, bitRead(intBit, 6));
      digitalWrite(TOP_BAR_6, bitRead(intBit, 7));
      digitalWrite(TOP_BAR_7, bitRead(intBit, 8));
      digitalWrite(TOP_BAR_8, bitRead(intBit, 9));
      digitalWrite(TOP_BAR_9, bitRead(intBit, 10));
      break;
  }
}

int getLightIndex() {
  int LDRRead = 1023 - analogRead(LDR);
  
  // gets only half the resolution to adapt for my dark ambient light
  int LDRIndex = map(LDRRead, 0, 512, 0, 11);
  
  if (LDRIndex > 10) {
    LDRIndex = 10;
  }
  return LDRIndex;
}

void lampOn() {
  for (int i = 0; i < 4; i++) {
    irsend.sendNEC(0xFFE01F, 32);
    delay(BASE_DELAY / 5);
  }
}

void lampMax() {
  for (int i = 0; i < 16; i++) {
    irsend.sendNEC(0xFFA05F, 32);
    delay(BASE_DELAY / 5);
  }
}

void beep() {
  tone(BUZZER, NOTE_A4, 300);
}

void playHPTheme() {
  while (true) {
    alarmSwitch = digitalRead(ALARM_BTN);

    int size = sizeof(hpTheme) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
      int noteDuration = 700 / hpTempo[thisNote];
      tone(BUZZER, hpTheme[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.25;
      delay(pauseBetweenNotes);

      alarmSwitch = digitalRead(ALARM_BTN);
      if (alarmSwitch) {
        break;
      }
    }
    alarmSwitch = digitalRead(ALARM_BTN);
    if (alarmSwitch) {
      break;
    }
  }
}

void playMarioTheme() {
  while (true) {
    alarmSwitch = digitalRead(ALARM_BTN);

    int size = sizeof(marioTheme) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
      int noteDuration = 8000 / marioTempo[thisNote];
      tone(BUZZER, marioTheme[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 0.2;
      delay(pauseBetweenNotes);

      alarmSwitch = digitalRead(ALARM_BTN);
      if (alarmSwitch) {
        break;
      }
    }
    alarmSwitch = digitalRead(ALARM_BTN);
    if (alarmSwitch) {
      break;
    }
  }
}

void playPokeIntroTheme() {
  while (true) {
    alarmSwitch = digitalRead(ALARM_BTN);

    int size = sizeof(pokeIntroTheme) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
      int noteDuration = 1000 / pokeIntroTempo[thisNote];
      tone(BUZZER, pokeIntroTheme[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.1;
      delay(pauseBetweenNotes);

      alarmSwitch = digitalRead(ALARM_BTN);
      if (alarmSwitch) {
        break;
      }
    }
    alarmSwitch = digitalRead(ALARM_BTN);
    if (alarmSwitch) {
      break;
    }
  }
}

void playSkyrimTheme() {
  while (true) {
    alarmSwitch = digitalRead(ALARM_BTN);

    int size = sizeof(skyrimTheme) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
      int noteDuration = 2000 / skyrimTempo[thisNote];
      tone(BUZZER, skyrimTheme[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.2;
      delay(pauseBetweenNotes);

      alarmSwitch = digitalRead(ALARM_BTN);
      if (alarmSwitch) {
        break;
      }
    }
    alarmSwitch = digitalRead(ALARM_BTN);
    if (alarmSwitch) {
      break;
    }
  }
}

void playZeldaTheme() {
  while (true) {
    alarmSwitch = digitalRead(ALARM_BTN);

    int size = sizeof(zeldaTheme) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
      int noteDuration = 2000 / zeldaTempo[thisNote];
      tone(BUZZER, zeldaTheme[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.1;
      delay(pauseBetweenNotes);

      alarmSwitch = digitalRead(ALARM_BTN);
      if (alarmSwitch) {
        break;
      }
    }
    alarmSwitch = digitalRead(ALARM_BTN);
    if (alarmSwitch) {
      break;
    }
  }
}

void playAlarm(int tune) {

  switch (tune) {
    case 0:
      // picks random tune
      randomTune = random(1, 6);
      playAlarm(randomTune);
      break;
    case 1:
      playHPTheme();
      break;
    case 2:
      playMarioTheme();
      break;
    case 3:
      playPokeIntroTheme();
      break;
    case 4:
      playSkyrimTheme();
      break;
    case 5:
      playZeldaTheme();
      break;
  }
}
