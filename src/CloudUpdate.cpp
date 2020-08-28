#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include "CloudUpdate.h"

update_result_t cloudUpdate(const char *script_url, int8_t ledPin, bool ledLevel, bool reboot) {
  update_result_t result = UPDATE_BADURL;

  if (WiFi.isConnected()) {
    WiFiClientSecure client;
    HTTPClient http;
    String url;

    client.setInsecure();
    url = FPSTR(script_url);
    url.concat(F("?platform=ESP8266&mac="));
    url.concat(WiFi.macAddress());
    url.concat(F("&md5="));
    url.concat(ESP.getSketchMD5());
    if (http.begin(client, url)) {
      http.setTimeout(15000);
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      if (http.GET() == HTTP_CODE_OK) {
        url = http.getString();
        http.end();
        if (url.length()) {
          if (http.begin(client, url)) {
            if (http.GET() == HTTP_CODE_OK) {
              int size = http.getSize();

              if (size > 0) {
                if (size <= (int)ESP.getFreeSketchSpace()) {
                  WiFiClient *tcp = http.getStreamPtr();

                  WiFiUDP::stopAll();
                  WiFiClient::stopAllExcept(tcp);

                  uint8_t header[4];

                  if (tcp->peekBytes(header, sizeof(header)) == sizeof(header)) {
                    if (header[0] == 0xE9) {
                      if (ESP.magicFlashChipSize((header[3] & 0xF0) >> 4) <= ESP.getFlashChipRealSize()) {
                        result = UPDATE_FAIL;
                        if (Update.begin(size, U_FLASH, ledPin, ledLevel)) {
                          if (Update.writeStream(*tcp) == (size_t)size) {
                            if (Update.end()) {
                              if (reboot) {
                                http.end();
                                ESP.restart();
                              }
                              result = UPDATE_OK;
                            }
                          }
                        }
                      } else
                        result = UPDATE_NO_SPACE;
                    }
                  }
                } else
                  result = UPDATE_NO_SPACE;
              }
            }
            http.end();
          }
        } else
          result = UPDATE_NONE;
      }
    }
  }
  return result;
}
