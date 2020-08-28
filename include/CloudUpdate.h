#pragma once

#include <Arduino.h>

enum update_result_t { UPDATE_OK, UPDATE_NONE, UPDATE_BADURL, UPDATE_NO_SPACE, UPDATE_FAIL };

update_result_t cloudUpdate(const char *script_url, int8_t ledPin = -1, bool ledLevel = LOW, bool reboot = true);
