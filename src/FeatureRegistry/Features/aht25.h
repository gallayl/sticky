#pragma once

#include <Wire.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#include "../../config.h"
#include "../Feature.h"
#include "../../CommandInterpreter/CustomCommand.h"
#include "../../CommandInterpreter/CommandParser.h"
#include "../../CommandInterpreter/CommandInterpreter.h"
#include "../../services/WebServer.h"

#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

String readTemperature()
{
    JsonDocument doc = JsonDocument().as<JsonObject>();

    aht.getStatus();

    aht.getTemperatureSensor()->enableAutoRange(true);

    sensors_event_t temp;
    aht.getTemperatureSensor()->getEvent(&temp);

    doc["temperature"] = temp.temperature;

    aht.getHumiditySensor()->enableAutoRange(true);
    sensors_event_t humidity;
    aht.getHumiditySensor()->getEvent(&humidity);

    doc["humidity"] = humidity.relative_humidity;

    char buffer[JSON_BUFFER_SIZE];
    serializeJson(doc, buffer);
    return String(buffer);
}

CustomCommand *aht25Command = new CustomCommand("aht25", [](String command)
                                                {
    String sub = CommandParser::GetCommandParameter(command, 1);

    if (sub == "read")
    {
        return readTemperature();
    }


    String fallback = "The awailable AHT25 Commands are: read";
    return fallback; });

Feature *aht25Feature = new Feature("aht25", []()
                                    {


    aht.begin(&Wire );

    CommandInterpreterInstance->RegisterCommand(*aht25Command);

    server.on("/aht25", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, MIME_json, readTemperature());
    });

    return FeatureState::RUNNING; }, []() {});