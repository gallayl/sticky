#pragma once

#include <ESPAsyncWebServer.h>
#include "./Logging.h"
#include "../Feature.h"
#include "../../CommandInterpreter/CustomCommand.h"
#include "../../CommandInterpreter/CommandInterpreter.h"
#include "../../services/WebServer.h"
#include "../../services/WebSocketServer.h"
#include "../../utils/Json.h"

#define PIR_PIN D6

bool getPirState()
{
  return digitalRead(PIR_PIN) == HIGH;
}

struct PirStateHistoryEntry
{
  bool state;
  unsigned long epochTime;
};

#define PIR_HISTORY_SIZE 25

std::vector<PirStateHistoryEntry> pirStateHistory;

volatile bool pirStateChanged = false;
volatile bool pirLastState = false;

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
  pirLastState = digitalRead(PIR_PIN) == HIGH;
  pirStateChanged = true;
}

void processPirStateChange()
{
  if (!pirStateChanged)
  {
    return;
  }
  pirStateChanged = false;
  bool state = pirLastState;

  if (state)
  {
    LoggerInstance->Info("PIR movement detected");
    webSocket->textAll("{\"type\":\"pir\",\"state\":true}");
  }
  else
  {
    LoggerInstance->Info("PIR movement stopped");
    webSocket->textAll("{\"type\":\"pir\",\"state\":false}");
  }
  pushToPirStateHistory(state);
}

CustomCommand *getPirStateCommand = new CustomCommand("getPirState", [](String command)
                                                      {
  JsonDocument response;
  response["state"] = getPirState();
  return jsonToString(response); });

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

  return FeatureState::RUNNING; }, []() {
    processPirStateChange();
  });
