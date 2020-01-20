#include "services.h"

#if MQTT_SUPPORT

#include <MQTTStatus.h>

MQTTStatus::MQTTStatus(AsyncWebServer* server, SecurityManager* securityManager) {
  server->on(MQTT_STATUS_SERVICE_PATH,
             HTTP_GET,
             securityManager->wrapRequest(std::bind(&MQTTStatus::mqttStatus, this, std::placeholders::_1),
                                          AuthenticationPredicates::IS_AUTHENTICATED));
}

void MQTTStatus::mqttStatus(AsyncWebServerRequest* request) {
  AsyncJsonResponse* response = new AsyncJsonResponse(false, MAX_MQTT_STATUS_SIZE);
  JsonObject root = response->getRoot();

  // Config config;
  root["active"] = MQTT.connected();
  root["ip_address"] = "";  //_mqtt_host.c_str();

  response->setLength();
  request->send(response);
}

#endif