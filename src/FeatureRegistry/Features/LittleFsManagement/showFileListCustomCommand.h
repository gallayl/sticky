#pragma once

#include <ArduinoJson.h>
#include <LittleFS.h>
#include "../../../CommandInterpreter/CustomCommand.h"
#include "../../../api/list.h"
#include "../Logging.h"

#define LOG_BUFFER_LENGTH 1024

CustomCommand *showFileListCustomCommand = new CustomCommand("list", [](String command) {
    JsonDocument response = JsonDocument().to<JsonArray>();

    File root = LittleFS.open("/", "r");

    if (!root)
    {
        LoggerInstance->Error(F("Failed to open directory"));
        return String("");
    }

    File file = root.openNextFile();

    while (file) {
        JsonObject fileObject = JsonObject();
        fileObject["name"] = file.name();
        fileObject["size"] = file.size();
        response.add(fileObject);
    }

    char buffer[LOG_BUFFER_LENGTH];
    serializeJson(response, buffer);
    return String(buffer); });
