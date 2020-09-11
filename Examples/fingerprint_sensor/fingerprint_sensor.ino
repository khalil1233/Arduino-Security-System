#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

SoftwareSerial ser(3, 2);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&ser);

int check;

void setup() {
  check = -1;
  Serial.begin(9600);
  finger.begin(57600);
a:;
  if (finger.verifyPassword()) {
    Serial.println("Sensor Connected!");
  } else {
    Serial.println("Can't Find Sernsor");
    delay(2000);
    goto a;
  }
  Serial.println("\nE: Enroll\nC: Check\nF: Get First Empty Id\nD: Delete Finger Template");
}

void loop() {
  if (Serial.available()) {
    char in = Serial.read();
    if (in == 'E') {
      enroll();
      Serial.println("\n\n\nE: Enroll\nC: Check\nF: Get First Empty Id\nD: Delete Finger Template");
    } else if (in == 'C') {
      scan();
      Serial.println("\n\n\nE: Enroll\nC: Check\nF: Get First Empty Id\nD: Delete Finger Template");
    } else if (in == 'F') {
      Serial.print(getEmptyID());
      Serial.println(" Is Empty");
      Serial.println("\n\n\nE: Enroll\nC: Check\nF: Get First Empty Id\nD: Delete Finger Template");
    } else if (in == 'D') {
      deleteTemplate();
      Serial.println("\n\n\nE: Enroll\nC: Check\nF: Get First Empty Id\nD: Delete Finger Template");
    }
  }
}

void getImage2Tz(int times = 1) {
  for (int i = 1; i <= times; i++) {
    check = -1;
    while (check != FINGERPRINT_OK) {
      check = finger.getImage();
      if (check == FINGERPRINT_NOFINGER) {
        Serial.println("Put Your Finger On The Sensor");
        while (check == FINGERPRINT_NOFINGER) {
          check = finger.getImage();
        }
      }
      switch (check) {
        case FINGERPRINT_OK:
          Serial.println("Image taken");
          break;
        case FINGERPRINT_PACKETRECIEVEERR:
          Serial.println("Communication error");
          break;
        case FINGERPRINT_IMAGEFAIL:
          Serial.println("Imaging error");
          break;
        default:
          Serial.println("Unknown error");
          break;
      }
      if (check == FINGERPRINT_OK) {
        check = finger.image2Tz(i);
        switch (check) {
          case FINGERPRINT_OK:
            Serial.println("Image converted");
            break;
          case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            break;
          case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            break;
          case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            break;
          case FINGERPRINT_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            break;
          default:
            Serial.println("Unknown error");
            break;
        }
      }
      Serial.println("Remove Your Finger");
      while (finger.getImage() != FINGERPRINT_NOFINGER && times == 2);
    }
  }
}

bool createStoreModel(int id) {
  Serial.println("Creating Model");
  check = finger.createModel();
  bool res = false;
  do {
    switch (check) {
      case FINGERPRINT_OK:
        Serial.println("Prints matched!");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        res = !res;
        break;
      case FINGERPRINT_ENROLLMISMATCH:
        Serial.println("Fingerprints did not match");
        break;
      default:
        Serial.println("Unknown error");
        res = !res;
        break;
    }
  } while (res);
  if (check == FINGERPRINT_OK) {
    Serial.println("Storing Model");
    finger.storeModel(id);
    res = false;
    do {
      switch (check) {
        case FINGERPRINT_OK:
          Serial.println("Stored!");
          break;
        case FINGERPRINT_PACKETRECIEVEERR:
          Serial.println("Communication error");
          res = !res;
          break;
        case FINGERPRINT_BADLOCATION:
          Serial.println("Could not store in that location");
          break;
        case FINGERPRINT_FLASHERR:
          Serial.println("Error writing to flash");
          res = !res;
          break;
        default:
          Serial.println("Unknown error");
          break;
      }
    } while (res);
    if (check == FINGERPRINT_OK)
      return true;
    else
      return false;
  } else return false;
}

void enroll() {
  int id = readId();

  do {
    getImage2Tz(2);
  } while (!createStoreModel(id));

}

void scan() {
  getImage2Tz();
  check = finger.fingerFastSearch();
  switch (check) {
    case FINGERPRINT_OK:
      Serial.print("Found a print match!\nFinger Id = ");
      Serial.println(finger.fingerID);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_NOTFOUND:
      Serial.println("Did not find a match");
      break;
    default:
      Serial.println("Unknown error");
      break;
  }
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

void deleteTemplate() {
  int id = readId();
  check = finger.deleteModel(id);

  if (check == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else
    Serial.println("Failed To Delete");
}

int readId() {
  Serial.println("Enter Finger Id 1-127");
  int id = 0;
  while (id == 0) {
    if (Serial.available()) {
      id = Serial.parseInt();
    }
  }
  return id;
}
