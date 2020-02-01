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
  _settings.enabled = root["enabled"];
  _settings.host = root["host"].as<String>();
  _settings.base = root["base"].as<String>();
  _settings.port = root["port"];
  _settings.retain = root["retain"];
  _settings.keepalive = root["keepalive"];
  _settings.qos = root["qos"];
  _settings.will = root["will"];
  _settings.username = root["username"].as<String>();
  _settings.password = root["password"].as<String>();
  _settings.heartbeat = root["heartbeat"];

  Serial.print("MQTTSettingsService::readFromJsonObject(JsonObject& root)");
  Serial.println(_settings.enabled);
  Serial.println(_settings.host);
  Serial.println(_settings.base);
  Serial.println(_settings.port);
  Serial.println(_settings.retain);
  Serial.println(_settings.keepalive);
  Serial.println(_settings.qos);
  Serial.println(_settings.will);
  Serial.println(_settings.username);
  Serial.println(_settings.password);
  Serial.println(_settings.heartbeat);
}

void MQTTSettingsService::writeToJsonObject(JsonObject& root) {
  root["enabled"] = _settings.enabled;
  root["host"] = _settings.host;
  root["base"] = _settings.base;
  root["port"] = _settings.port;
  root["retain"] = _settings.retain;
  root["keepalive"] = _settings.keepalive;
  root["qos"] = _settings.qos;
  root["will"] = _settings.will;
  root["username"] = _settings.username;
  root["password"] = _settings.password;
  root["heartbeat"] = _settings.heartbeat;
}

#endif
