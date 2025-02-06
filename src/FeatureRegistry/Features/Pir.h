#pragma once

#include <ESPAsyncWebServer.h>
#include "./Logging.h"
#include "../Feature.h"
#include "../../CommandInterpreter/CustomCommand.h"
#include "../../CommandInterpreter/CommandInterpreter.h"
#include "../../services/WebServer.h"
#include "../../services/WebSocketServer.h"

#define PIR_PIN D6

bool getPirState()
{
  return digitalRead(PIR_PIN) == HIGH ? true : false;
}

struct PirStateHistoryEntry
{
  bool state;
  unsigned long epochTime;
};

#define PIR_HISTORY_SIZE 25

std::vector<PirStateHistoryEntry> pirStateHistory;

bool getDebouncedPirState(unsigned long timeToDebounce)
{
  bool currentState = getPirState();
  if (currentState == true)
  {
    return true;
  }

  unsigned long now = millis();

  for (int i = pirStateHistory.size() - 1; i >= 0; i--)
  {
    if (pirStateHistory[i].state == true && now - pirStateHistory[i].epochTime < timeToDebounce)
    {
      return true;
    }
  }

  return false;
}

void pushToPirStateHistory(bool state)
{
  pirStateHistory.push_back({state, millis()});
  if (pirStateHistory.size() > PIR_HISTORY_SIZE)
  {
    pirStateHistory.erase(pirStateHistory.begin());
  }
}

void IRAM_ATTR pirMovement()
{
  if (digitalRead(PIR_PIN) == HIGH)
  {
    LoggerInstance->Info("PIR movement detected");
    webSocket->textAll("{\"type\":\"pir\",\"state\":true}");
    pushToPirStateHistory(true);
  }
  else
  {
    LoggerInstance->Info("PIR movement stopped");
    webSocket->textAll("{\"type\":\"pir\",\"state\":false}");
    pushToPirStateHistory(false);
  }
}

CustomCommand *getPirStateCommand = new CustomCommand("getPirState", [](String command)
                                                      {
  JsonDocument response = JsonDocument().to<JsonObject>();
  response["state"] = getPirState();
  char buffer[JSON_BUFFER_SIZE];
  serializeJson(response, buffer);
  return String(buffer); });

ArRequestHandlerFunction getPirStateAction = [](AsyncWebServerRequest *request)
{
  AsyncJsonResponse *resp = new AsyncJsonResponse();
  JsonDocument response;
  response["state"] = getPirState();
  resp->setCode(200);
  resp->getRoot().set(response);
  resp->setLength();
  request->send(resp);
};

ArRequestHandlerFunction getDebouncedPirStateAction = [](AsyncWebServerRequest *request)
{
  AsyncJsonResponse *resp = new AsyncJsonResponse();
  JsonDocument response;
  unsigned long timeToDebounce = request->hasParam("timeToDebounce") ? request->getParam("timeToDebounce")->value().toInt() : 60000; // 1 min by default
  response["state"] = getDebouncedPirState(timeToDebounce);
  resp->setCode(200);
  resp->getRoot().set(response);
  resp->setLength();
  request->send(resp);
};

Feature *PirFeature = new Feature("pir", []()
                                  {
    attachInterrupt(PIR_PIN, pirMovement, CHANGE);
    CommandInterpreterInstance->RegisterCommand(*getPirStateCommand);
    server.on("/pir", HTTP_GET, getPirStateAction);

    server.on("/pir-debounced", HTTP_GET, getDebouncedPirStateAction);

  return FeatureState::RUNNING; }, []() {});
