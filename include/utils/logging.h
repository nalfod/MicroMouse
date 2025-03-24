// Micromouse course - evosoft
#pragma once

#ifdef LOGGING_ENABLED
    #include <Arduino.h>
    #include <ArduinoLog.h>
    #include <BleSerial.h>

#ifdef BLE_LOGGING
    #define LOGGING_BEGIN() {                       \
        ble.begin("EvoMouseWZV");                      \
        Log.begin(LOG_LEVEL_VERBOSE, &ble);         \
    }
#else
    #define LOGGING_BEGIN() {                       \
        Serial.begin(460800);                       \
        Log.begin(LOG_LEVEL_VERBOSE, &Serial);      \
        Serial.println("");                         \
    }
#endif
    #define LOG_FATAL(...) Log.fatal(__VA_ARGS__)
    #define LOG_ERROR(...) Log.error(__VA_ARGS__)
    #define LOG_WARNING(...) Log.warning(__VA_ARGS__)
    #define LOG_INFO(...) Log.notice(__VA_ARGS__)
    #define LOG_TRACE(...) Log.trace(__VA_ARGS__)
    #define LOG_DEBUG(...) Log.verbose(__VA_ARGS__)
#else
    #define LOGGING_BEGIN(...)
    #define LOG_FATAL(...)
    #define LOG_ERROR(...)
    #define LOG_WARNING(...)
    #define LOG_INFO(...)
    #define LOG_TRACE(...)
    #define LOG_DEBUG(...)
#endif