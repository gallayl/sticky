#pragma once

#include "../../CommandInterpreter/CustomCommand.h"
#include "../../CommandInterpreter/CustomCommand.h"
#include "../../hw/WiFi.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

#ifdef ESP32
#include <WiFi.h>
#include "../../FeatureRegistry/Features/esp32/Camera.h"
#else
#include <ESP8266WiFi.h>
#endif

#ifdef ESP32
String getResetReason()
{
    switch (esp_reset_reason())
    {
    case ESP_RST_UNKNOWN:
        return String("Reset reason can not be determined");
    case ESP_RST_POWERON:
        return String("Reset due to power-on event");
    case ESP_RST_EXT:
        return String("Reset by external pin (not applicable for ESP32)");
    case ESP_RST_SW:
        return String("Software reset via esp_restart");
    case ESP_RST_PANIC:
        return String("Software reset due to exception/panic");
    case ESP_RST_INT_WDT:
        return String("Reset (software or hardware) due to interrupt watchdog");
    case ESP_RST_TASK_WDT:
        return String("Reset due to task watchdog");
    case ESP_RST_WDT:
        return String("Reset due to other watchdogs");
    case ESP_RST_DEEPSLEEP:
        return String("Reset after exiting deep sleep mode");
    case ESP_RST_BROWNOUT:
        return String("Brownout reset (software or hardware)");
    case ESP_RST_SDIO:
        return String("Reset over SDIO");
    }
    return String("Unknown");
}
#endif

JsonDocument getInfo(){
    JsonDocument response;
    JsonObject esp = response["esp"].to<JsonObject>();

    esp["sdkVersion"] = ESP.getSdkVersion();
    esp["cpuFreqMhz"] = ESP.getCpuFreqMHz();
    esp["freeHeap"] = ESP.getFreeHeap();
    esp["freeSkSpace"] = ESP.getFreeSketchSpace();

    JsonObject flash = response["flash"].to<JsonObject>();

    flash["mode"] = ESP.getFlashChipMode();
    flash["size"] = ESP.getFlashChipSize();
    flash["speed"] = ESP.getFlashChipSpeed();

#ifdef ESP32
    esp["restartReson"] = getResetReason();
    esp["freePsRam"] = ESP.getFreePsram();

    JsonObject camera = response["camera"].to<JsonObject>();
    sensor_t *sensor = esp_camera_sensor_get();
    camera["framesize"] = sensor->status.framesize;
    camera["quality"] = sensor->status.quality;
    camera["errorCode"] = cameraErrorCode;
    

    JsonObject littleFs = response["littleFs"].to<JsonObject>();
    littleFs["totalBytes"] = LittleFS.totalBytes();
    littleFs["usedBytes"] = LittleFS.usedBytes();

#endif

    return response;
}

CustomCommand *infoCustomCommand = new CustomCommand("info", [](String command)
                                              {
    JsonDocument response = getInfo();

    char buffer[JSON_BUFFER_SIZE];
    serializeJson(response, buffer);
    return String(buffer); });