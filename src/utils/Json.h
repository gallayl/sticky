#pragma once

#include <ArduinoJson.h>

inline String jsonToString(const JsonDocument &doc)
{
    String out;
    serializeJson(doc, out);
    return out;
}
