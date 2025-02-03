#pragma once

#include "../../../CommandInterpreter/CustomCommand.h"
#include <LittleFS.h>

CustomCommand *formatCustomCommand = new CustomCommand("format", [](String command)
                                         {
    LittleFS.format();
    ESP.restart();
    return String("{\"event\": \"format\"}"); });