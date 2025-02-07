#pragma once

#include "../../CommandInterpreter/CustomCommand.h"
#include "../../CommandInterpreter/CustomCommand.h"
#include "../../hw/WiFi.h"
#include <LittleFS.h>

#include <ESP8266WiFi.h>

JsonDocument getInfo()
{
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

    FSInfo fsInfo;
    LittleFS.info(fsInfo);

    JsonObject fs = response["fs"].to<JsonObject>();
    fs["totalBytes"] = fsInfo.totalBytes;
    fs["usedBytes"] = fsInfo.usedBytes;
    fs["blockSize"] = fsInfo.blockSize;
    fs["pageSize"] = fsInfo.pageSize;
    fs["maxOpenFiles"] = fsInfo.maxOpenFiles;
    fs["maxPathLength"] = fsInfo.maxPathLength;

    return response;
}

CustomCommand *infoCustomCommand = new CustomCommand("info", [](String command)
                                                     {
    JsonDocument response = getInfo();

    char buffer[JSON_BUFFER_SIZE];
    serializeJson(response, buffer);
    return String(buffer); });