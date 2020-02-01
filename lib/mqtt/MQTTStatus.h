#ifndef MQTTStatus_h
#define MQTTStatus_h
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(ESP32)
#include <FreeRTOS.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#endif

#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <IPAddress.h>
#include <SecurityManager.h>

#include <Config.h>
#include <MQTTClient.h>

#define MAX_MQTT_STATUS_SIZE 1024
#define MQTT_STATUS_SERVICE_PATH "/rest/mqttStatus"

class MQTTStatus {
 public:
  MQTTStatus(AsyncWebServer* server, SecurityManager* securityManager);

 private:
  void mqttStatus(AsyncWebServerRequest* request);
};

#endif
