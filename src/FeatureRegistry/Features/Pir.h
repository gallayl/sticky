#pragma once

#include <ESPAsyncWebServer.h>
#include "./Logging.h"
#include "../Feature.h"
#include "../../CommandInterpreter/CustomCommand.h"
#include "../../CommandInterpreter/CommandInterpreter.h"
#include "../../services/WebServer.h"
#include "../../services/WebSocketServer.h"

#define PIR_PIN D6

void IRAM_ATTR  pirMovement() {
  if (digitalRead(PIR_PIN) == HIGH) {
    LoggerInstance->Info("PIR movement detected");
    webSocket->textAll("{\"type\":\"pir\",\"state\":true}");
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    LoggerInstance->Info("PIR movement stopped");
    webSocket->textAll("{\"type\":\"pir\",\"state\":false}");
    digitalWrite(LED_BUILTIN, LOW);
  }
}


CustomCommand *getPirState = new CustomCommand("getPirState", [](String command) {
  JsonDocument response = JsonDocument().to<JsonObject>();
  response["state"] = digitalRead(PIR_PIN) == HIGH ? true : false;
  char buffer[64];
  serializeJson(response, buffer);
  return String(buffer);
});


ArRequestHandlerFunction getPirStateAction = [](AsyncWebServerRequest *request) {
    AsyncJsonResponse * resp = new AsyncJsonResponse();
    JsonObject response = JsonObject();
    response["state"] = digitalRead(PIR_PIN);
    resp->setCode(200);
    resp->getRoot().set(response);
    resp->setLength();
    request->send(resp);
};




Feature* PirFeature = new Feature("pir", []() {
    attachInterrupt(PIR_PIN, pirMovement, CHANGE);
    CommandInterpreterInstance->RegisterCommand(*getPirState);
    server.on("/pir", HTTP_GET, getPirStateAction);

  return FeatureState::RUNNING;
}, [](){});
