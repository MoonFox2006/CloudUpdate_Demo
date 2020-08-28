#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "CloudUpdate.h"

const uint8_t BTN_PIN = 0;
const bool BTN_LEVEL = LOW;
const uint8_t LED_PIN = 16;
const bool LED_LEVEL = LOW;

const char WIFI_SSID[] PROGMEM = "******";
const char WIFI_PSWD[] PROGMEM = "******";

const char SCRIPT_URL[] PROGMEM = "https://script.google.com/macros/s/******/exec";

bool wifiConnect(const char *wifi_ssid, const char *wifi_pswd, uint32_t timeout = 30000) {
  char _wifi_ssid[strlen_P(wifi_ssid) + 1], _wifi_pswd[strlen_P(wifi_pswd) + 1];
  uint32_t time = millis();

  strcpy_P(_wifi_ssid, wifi_ssid);
  strcpy_P(_wifi_pswd, wifi_pswd);
  WiFi.begin(_wifi_ssid, _wifi_pswd);
  Serial.print(F("Connecting to \""));
  Serial.print(_wifi_ssid);
  Serial.print('"');
  while ((! WiFi.isConnected()) && (millis() - time < timeout)) {
    digitalWrite(LED_PIN, LED_LEVEL);
    delay(25);
    digitalWrite(LED_PIN, ! LED_LEVEL);
    delay(500 - 25);
    Serial.print('.');
  }
  if (WiFi.isConnected()) {
    Serial.print(F(" OK (IP "));
    Serial.print(WiFi.localIP());
    Serial.println(')');
    return true;
  } else {
    WiFi.disconnect();
    Serial.println(F(" FAIL!"));
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(BTN_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ! LED_LEVEL);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);

  if (wifiConnect(WIFI_SSID, WIFI_PSWD)) {
    update_result_t ret = cloudUpdate(SCRIPT_URL, LED_PIN, LED_LEVEL);

    switch (ret) {
      case UPDATE_NONE:
        Serial.println(F("Firmware is already actual"));
        break;
      case UPDATE_BADURL:
        Serial.println(F("Error getting firmware URL!"));
        break;
      case UPDATE_NO_SPACE:
        Serial.println(F("New firmware incompatible with flash!"));
        break;
      case UPDATE_FAIL:
        Serial.println(F("Firmware update fail!"));
        break;
      default:
        break;
    }
    WiFi.disconnect();
  }
}

void loop() {
  static bool lastBtn = false;

  if ((digitalRead(BTN_PIN) == BTN_LEVEL) != lastBtn) {
    lastBtn = ! lastBtn;
    digitalWrite(LED_PIN, LED_LEVEL == lastBtn);
  }
}
