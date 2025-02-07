#pragma once

#include <Wire.h>

#include "../../config.h"
#include "./Logging.h"
#include "../Feature.h"
#include "../../CommandInterpreter/CustomCommand.h"
#include "../../CommandInterpreter/CommandParser.h"
#include "../../CommandInterpreter/CommandInterpreter.h"
#include "../../services/WebServer.h"

#include <Adafruit_AHTX0.h>

#define AHT25_READ_INTERVAL 5000

Adafruit_AHTX0 aht;

struct Reading
{
    float temperature;
    float humidity;
    unsigned long timestamp;
};

Reading lastReading = {0, 0, 0};

Reading getTemperatureFromSensor()
{
    sensors_event_t temp;
    sensors_event_t humidity;
    aht.getEvent(&humidity, &temp);

    Reading reading;
    reading.temperature = temp.temperature;
    reading.humidity = humidity.relative_humidity;
    reading.timestamp = millis();

    return reading;
}

String readTemperature()
{

    JsonDocument doc = JsonDocument().as<JsonObject>();

    if (lastReading.timestamp == 0 || millis() - lastReading.timestamp > AHT25_READ_INTERVAL)
    {
        lastReading = getTemperatureFromSensor();
    }

    doc["temperature"] = lastReading.temperature;
    doc["humidity"] = lastReading.humidity;
    doc["timestamp"] = lastReading.timestamp;

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


    if (!aht.begin(&Wire )){
        LoggerInstance->Error("Failed to init AHT25 sensor");
        return FeatureState::ERROR;
    }

    CommandInterpreterInstance->RegisterCommand(*aht25Command);

    server.on("/aht25", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, MIME_json, readTemperature());
    });

    return FeatureState::RUNNING; }, []() {});