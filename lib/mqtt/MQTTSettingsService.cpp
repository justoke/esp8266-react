#include "services.h"

#if MQTT_SUPPORT

#include <MQTTClient.h>
#include <MQTTSettingsService.h>

MQTTSettingsService::MQTTSettingsService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager) :
    AdminSettingsService(server, fs, securityManager, MQTT_SETTINGS_SERVICE_PATH, MQTT_SETTINGS_FILE) {
}

MQTTSettingsService::~MQTTSettingsService() {
}

void MQTTSettingsService::begin() {
  Serial.println("MQTTSettingsService::begin)");
  AdminSettingsService::begin();
  MQTT.setup(this);
}

void MQTTSettingsService::loop() {
  // Serial.print("MQTTSettingsService::loop()");
  MQTT.loop();
}

void MQTTSettingsService::onConfigUpdated() {
  Serial.println("MQTTSettingsService::onConfigUpdated()");
  MQTT.setup(this);
}

void MQTTSettingsService::readFromJsonObject(JsonObject& root) {
  /*
    Config::mqtt_enabled = _enabled = root["enabled"];
  Config::mqtt_host = _host = root["host"].as<String>();
  Config::mqtt_base = _base = root["base"].as<String>();
  Config::mqtt_port = _port = root["port"];
  Config::mqtt_retain = _retain = root["retain"];
  Config::mqtt_keepalive = _keepalive = root["keepalive"];
  Config::mqtt_qos = _qos = root["qos"];
  Config::mqtt_will = _will = root["will"];
  Config::mqtt_username = _username = root["username"].as<String>();
  Config::mqtt_password = _password = root["password"].as<String>();
  Config::mqtt_heartbeat = _heartbeat = root["heartbeat"];
  

  */
  _enabled = root["enabled"];
  _host = root["host"].as<String>();
  _base = root["base"].as<String>();
  _port = root["port"];
  _retain = root["retain"];
  _keepalive = root["keepalive"];
  _qos = root["qos"];
  _will = root["will"];
  _username = root["username"].as<String>();
  _password = root["password"].as<String>();
  _heartbeat = root["heartbeat"];

  Serial.print("MQTTSettingsService::readFromJsonObject(JsonObject& root)");
  Serial.println(_enabled);
  Serial.println(_host);
  Serial.println(_base);
  Serial.println(_port);
  Serial.println(_retain);
  Serial.println(_keepalive);
  Serial.println(_qos);
  Serial.println(_will);
  Serial.println(_username);
  Serial.println(_password);
  Serial.println(_heartbeat);
}

void MQTTSettingsService::writeToJsonObject(JsonObject& root) {
  root["enabled"] = _enabled;
  root["host"] = _host;
  root["base"] = _base;
  root["port"] = _port;
  root["retain"] = _retain;
  root["keepalive"] = _keepalive;
  root["qos"] = _qos;
  root["will"] = _will;
  root["username"] = _username;
  root["password"] = _password;
  root["heartbeat"] = _heartbeat;
}

#endif
