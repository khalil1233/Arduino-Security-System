#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>

#define fp_tx 2
#define fp_rx 3

#define ok_bu A2
#define nxt_bu A1
#define prv_bu A0

#define in_bu A3

#define relay 5

#define speaker 7

#define mg_sw 8

SoftwareSerial ser(fp_rx, fp_tx);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&ser);

LiquidCrystal_I2C my_lcd(0x27, 16, 2);

byte arrow[] = {
  B00000,
  B00000,
  B10001,
  B10001,
  B01010,
  B01010,
  B00100,
  B00000
};

const int e5 = 659, d5s = 622, b4 = 493;
const int d5 = 587, c5 = 523, a4 = 440;
const int c4 = 261, e4 = 329, g4s = 415;

unsigned long timer;
int check;
int ft;

void lcd(String l1 = "", String l2 = "") {
  my_lcd.clear();
  my_lcd.setCursor(0, 0);
  my_lcd.print(l1);
  my_lcd.setCursor(0, 1);
  my_lcd.print(l2);
}

void furElise() {
  ft = 300;
  bool s = false;
  for (int ii = 0 ; ii < 2; ii++) {
    for (int i = 0; i < 2; i++) {
      note(e5);
      note(d5s);
    }
    note(e5);
    note(b4);
    note(d5);
    note(c5);
    note(a4);
    note(c4);
    note(e4);
    note(a4);
    note(b4);
    if (!s) {
      s = !s;
      note(e4);
      note(g4s);
      note(b4);
      note(c5);
    } else {
      s = !s;
      note(e4);
      note(c5);
      note(b4);
      note(a4);
    }
    note(e4);
  }
}

void note(int n) {
  tone(speaker, n);
  delay(ft);
  if (digitalRead(mg_sw) || digitalRead(ok_bu) == LOW) {
    ft = 0;
  }
  if (!digitalRead(in_bu)) {
    ft = 0;
    unlock();
  }
}

void lcdHome() {
  lcd("Ring The Bell");
  my_lcd.write(0);
}

void unlock() {
  timer = millis() + 5000;
  digitalWrite(relay, LOW);
  tone(speaker, 50);
  delay(2000);
  lcd("Lock The Door");
  my_lcd.write(0);
  while (timer > millis() && digitalRead(prv_bu) && !digitalRead(mg_sw)) {
    yield();
  }
  if (digitalRead(mg_sw)) {
    lcd("Don't Forget To", "Close The Door!");
    delay(500);
  }
  lock();
  while (digitalRead(mg_sw)) {
    yield();
  }
  lcdHome();
  delay(500);
}

void lock() {
  digitalWrite(relay, HIGH);
  noTone(speaker);
}

int getEmptyID() {
  finger.getTemplateCount();
  if (finger.templateCount < 127) {
    for (int i = 1; i <= 127; i++) {
      if (finger.loadModel(i) != FINGERPRINT_OK) {
        return i;
        break;
      }
    }
  } else return -1;
}

bool getImg2Tz(int times = 1) {
  check = finger.getImage();
  if (check != FINGERPRINT_NOFINGER) {
    if (check == FINGERPRINT_OK) {
      check = finger.image2Tz(times);
    }
    else check = -1;
    lcd("Remove Your", "Finger");
    while (finger.getImage() != FINGERPRINT_NOFINGER)yield();
    return true;
  } else {
    return false;
  }
}

void enroll(bool noCancel = false) {
  int id = getEmptyID();
  if (id == -1) {
    lcd("Storage Full", "Cant Store More");
    delay(3000);
  } else {
    check = -1;
    while (check != FINGERPRINT_OK) {
      for (int i = 1; i <= 2; i++) {
        check = -1;
        if (!noCancel) {
          lcd("Put Your Finger", "          Cancel");
        } else {
          lcd("Enrolling", "Put Your Finger");
        }
        while (check != FINGERPRINT_OK) {
          getImg2Tz(i);
          if (check == -1) {
            if (!noCancel) {
              lcd("Put Your Finger", "          Cancel");
            } else {
              lcd("Enrolling", "Put Your Finger");
            }
          }
          if (digitalRead(ok_bu) == LOW && !noCancel) {
            goto e;
          }
        }
      }
      check = finger.createModel();
      if (check == FINGERPRINT_OK) {
        check = finger.storeModel(id);
      }
    }
    lcd("Finger Stored", "Your Id Is " + String(id));
    delay(3000);
  }
e:;
  if (!noCancel) {
    lcd("Enroll", "  >           OK");
  }
}

void setup() {
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  pinMode(speaker, OUTPUT);

  pinMode(ok_bu, INPUT_PULLUP);
  pinMode(nxt_bu, INPUT_PULLUP);
  pinMode(prv_bu, INPUT_PULLUP);
  pinMode(mg_sw, INPUT_PULLUP);
  pinMode(in_bu, INPUT_PULLUP);

  my_lcd.begin();
  finger.begin(57600);

  my_lcd.createChar(0, arrow);

  if (!finger.verifyPassword()) {
    lcd("Sensor Error:", "Fingerprint");
    while (!finger.verifyPassword()) {
      yield();
    }
  }
  finger.getTemplateCount();
  if (finger.templateCount < 1) {
    digitalWrite(relay, LOW);
    enroll(true);
    digitalWrite(relay, HIGH);
  }
  lcdHome();
}

bool checkFinger() {
  if (getImg2Tz()) {
    if (finger.fingerFastSearch() == FINGERPRINT_OK) {
      lcd("Door Unlocked", "ID = " + String(finger.fingerID));
      return true;
    } else {
      lcd("Unknown Finger");
      delay(2000);
      lcdHome();
      return false;
    }
  } else {
    return false;
  }
}

void refMenu(int p) {
  switch (p) {
    case 1:
      lcd("Enroll", "  >           OK");
      break;
    case 2:
      lcd("Remove", "< >           OK");
      break;
    case 3:
      lcd("Factory Reset", "< >           OK");
      break;
    case 4:
      lcd("Exit Menu", "<             OK");
  }
}

void deleteTemplate(int id, bool withConf = true) {
  check = finger.deleteModel(id);
  if (withConf) {
    if (check == FINGERPRINT_OK) {
      lcd("Deleted", "Successfully");
    } else {
      lcd("Delete Failed", "Try Again");
    }
    delay(2000);
  }
}

void remo() {
  finger.getTemplateCount();
  int len = finger.templateCount;
  if (len == 1) {
    lcd("There Is One Id", "Cant Delete It");
    delay(2000);
    lcd("Remove", "< >           OK");
  } else {
    int used[127], n = 0;
    for (int i = 1; i <= 127; i++) {
      if (finger.loadModel(i) == FINGERPRINT_OK) {
        used[n] = i;
        n++;
      }
    }
    n = 0;
    timer = millis() + 6000;
    lcd("Delete #" + String(used[n]), "X >          OK");
    while (timer > millis()) {
      if (digitalRead(prv_bu) == LOW) {
        if (n == 0) {
          timer = 0;
          break;
        } else if (n == 1) {
          n--;
          timer = millis() + 6000;
          lcd("Delete #" + String(used[n]), "X >           OK");
        } else {
          n--;
          timer = millis() + 6000;
          lcd("Delete #" + String(used[n]), "< >           OK");
        }
        delay(400);
      } else if (digitalRead(nxt_bu) == LOW) {
        if (!(n == len - 1)) {
          if (n == len - 2) {
            n++;
            lcd("Delete #" + String(used[n]), "<             OK");
          } else {
            n++;
            lcd("Delete #" + String(used[n]), "< >           OK");
          }
        }
        timer = millis() + 6000;
        delay(400);
      }
      else if (digitalRead(ok_bu) == LOW) {
        timer = millis() + 6000;
        deleteTemplate(used[n]);
        delay(2000);
        remo();
      }
    }
  }
}

void menu() {
  bool mloop = true;
  int page = 1;
  refMenu(page);
  timer = millis() + 6000;
  while (mloop) {
    if (digitalRead(prv_bu) == LOW && page > 1) {
      timer = millis() + 6000;
      page--;
      refMenu(page);
      delay(400);
    }
    else if (digitalRead(nxt_bu) == LOW && page < 4) {
      timer = millis() + 6000;
      page++;
      refMenu(page);
      delay(400);
    } else if (digitalRead(ok_bu) == LOW) {
      timer = millis() + 6000;
      if (page == 1) {
        enroll();
        timer = millis() + 6000;
        delay(400);
      } else if (page == 2) {
        remo();
        finger.getTemplateCount();
        if (finger.templateCount < 1) {
          digitalWrite(relay, LOW);
          enroll(true);
          digitalWrite(relay, HIGH);
        } else {
          timer = millis() + 6000;
        }
        delay(400);
      } else if (page == 3) {
        delay(400);
        lcd("Are You Sure?", "Cancel       Yes");
        timer = millis() + 5000;
        while (timer > millis()) {
          if (digitalRead(ok_bu) == LOW) {
            for (int i = 1; i < 128; i++) {
              deleteTemplate(i, false);
            }
            digitalWrite(relay, LOW);
            enroll(true);
            digitalWrite(relay, HIGH);
          } else if (digitalRead(prv_bu) == LOW) {
            break;
          }
        }
      } else if (page == 4) {
        mloop = false;
      }
    }
    if (millis() > timer) {
      mloop = false;
    }
    yield();
  }
}

void loop() {
  if (!digitalRead(in_bu)) {
    unlock();
  }
  if (digitalRead(mg_sw)) {
    bool st = false;
    while (!st) {
      int i = 200;
      while (i < 800 && !st) {
        i++;
        tone(7, i);
        delay(5);
        if (checkFinger())st = true;
      }
      delay(100);
    }
    lcdHome();
    noTone(7);
  }

  if (checkFinger()) {
    unlock();
  }

  if (digitalRead(prv_bu) == LOW) {
    lcd("Ringing", "           STOP");
    my_lcd.write(0);
    furElise();
    noTone(speaker);
    lcdHome();
  }

  if (digitalRead(ok_bu) == LOW) {
    delay(500);
    if (digitalRead(ok_bu) == LOW) {
      timer = millis() + 6000;
      lcd("Ready To Scan", "Settings");
      while (finger.getImage() == FINGERPRINT_NOFINGER) {
        if (!digitalRead(in_bu)) {
          unlock();
          break;
        }
        if (timer < millis()) {
          break;
        } else {
          yield();
        }
      }
      if (checkFinger()) {
        digitalWrite(relay, LOW);
        menu();
        digitalWrite(relay, HIGH);
      }
      lcdHome();
    }
  }
}
