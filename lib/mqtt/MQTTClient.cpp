#include "services.h"

#if MQTT_SUPPORT

#include "Config.h"
#include "MQTTClient.h"

#include <ArduinoJson.h>
#include <WiFiStatus.h>
#include "TimeLib.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <AsyncTCP.h>
#endif

// set callback for custom MQTT handlers
void MQTTClient::setMQTTcallback(mqtt_callback_f callback) {
  _mqtt_callback_f = callback;
}

void MQTTClient::onConnect() {
  // LOGGER.info(F("MQTT Connected"));
  Serial.println(F("MQTT Connected"));

  _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MIN;
  _mqtt_last_connection = millis();

  // say we're alive to the Last Will topic, using RETAIN on
  /*Config config;
  if (config.mqtt_will()) {
    (void)publish(uuid::read_flash_string(F_(mqtt_will_topic)).c_str(),
                  uuid::read_flash_string(F_(mqtt_will_payload_online)).c_str(),
                  true);
  }*/

  _heartbeatCheck(true);

  // call custom function to handle mqtt receives
  if (_mqtt_callback_f) {
    (_mqtt_callback_f)(MQTT_CONNECT_EVENT, nullptr, nullptr);
  }
}

void MQTTClient::_heartbeatCheck(bool force) {
  static uint32_t last_heartbeat = 0;

  if ((millis() - last_heartbeat > MQTT_HEARTBEAT_INTERVAL) || force) {
    last_heartbeat = millis();

    if (!mqttClient.connected() || !(_mqtt_heartbeat)) {
      return;
    }

    StaticJsonDocument<200> doc;
    JsonObject rootHeartbeat = doc.to<JsonObject>();
    rootHeartbeat["id"] = _hostname;
    rootHeartbeat["rssid"] = WiFiStatus::getWifiQuality();
    rootHeartbeat["timestamp"] = now();

    // rootHeartbeat["uptime"] = NTP.getUptime();  // uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);
    rootHeartbeat["freeheap"] = ESP.getFreeHeap();

    char data[300] = {0};
    serializeJson(doc, data, sizeof(data));

    // bool publish_ok = publish(uuid::read_flash_string(F_(mqtt_will_topic_heartbeat)).c_str(), data, false);

    bool publish_ok = publish("heartbeat", data, false);  // send to MQTT with retain off
    if (!publish_ok) {
      Serial.println(F("Hearbeat failed to send"));
    }

    Serial.println(F("MQTTClient::_heartbeatCheck .. Publish OK"));
  }
}

void MQTTClient::setup(MQTTSettingsService* mqttSettingsService) {
  Serial.println(F("MQTTClient::setup"));

  String config;
  mqttSettingsService->fetchAsString(config);
  DynamicJsonDocument jsonDocument(MAX_SETTINGS_SIZE);
  deserializeJson(jsonDocument, config);
  // mqttSettingsService->fetchAsDocument(jsonDocument);
  JsonObject root = jsonDocument.to<JsonObject>();

  // Config config;
  Serial.println(F("MQTTClient::configure"));
  _hostname = "UniqueDeviceId";
  _mqtt_enabled = root["enabled"];
  _mqtt_host = root["host"].as<String>();
  _mqtt_base = root["base"].as<String>();
  _mqtt_port = root["port"];
  _mqtt_retain = root["retain"];
  _mqtt_keepalive = root["keepalive"];
  _mqtt_qos = root["qos"];
  _mqtt_will = root["will"];
  _mqtt_username = root["username"].as<String>();
  _mqtt_password = root["password"].as<String>();
  _mqtt_heartbeat = root["heartbeat"];

  _mqtt_username = root["username"].as<String>();
  _mqtt_password = root["password"].as<String>();

  mqttClient.onConnect([this](bool sessionPresent) { onConnect(); });

  mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
    if (reason == AsyncMqttClientDisconnectReason::TCP_DISCONNECTED) {
      Serial.println(F("TCP Disconnected"));
    }
    if (reason == AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED) {
      Serial.println(F("Identifier Rejected"));
    }
    if (reason == AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE) {
      Serial.println(F("Server unavailable"));
    }
    if (reason == AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS) {
      Serial.println(F("Malformed credentials"));
    }
    if (reason == AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED) {
      Serial.println(F("Not authorized"));
    }

    // Reset reconnection delay
    _mqtt_last_connection = millis();
    _mqtt_connecting = false;

    if (_mqtt_callback_f) {
      (_mqtt_callback_f)(MQTT_DISCONNECT_EVENT, nullptr, nullptr);  // call callback with disconnect
    }
  });

  mqttClient.onMessage([this](char* topic,
                              char* payload,
                              AsyncMqttClientMessageProperties properties,
                              size_t len,
                              size_t index,
                              size_t total) { onMessage(topic, payload, len); });

  // configure the settings
  configure(false);

  _mqtt_connecting = false;
  _mqtt_last_connection = millis();
}

void MQTTClient::configure(bool reconnect) {
  // Config config;
  Serial.println(F("MQTTClient::configure"));
  /*   _mqtt_enabled = _mqtt_enabled;
    _hostname = _host_id.length() == 0 ? Config::getDeviceId() : Config::host_id;
    _mqtt_base = Config::mqtt_base;
    _mqtt_retain = Config::mqtt_retain;
    _mqtt_qos = Config::mqtt_qos;
    _mqtt_heartbeat = Config::mqtt_heartbeat; */

  Serial.print("MQTTClient::configure()");
  Serial.println(_mqtt_enabled);
  Serial.println(_hostname);
  Serial.println(_mqtt_base);
  Serial.println(_mqtt_port);
  Serial.println(_mqtt_retain);
  Serial.println(_mqtt_keepalive);
  Serial.println(_mqtt_qos);
  Serial.println(_mqtt_will);
  Serial.println(_mqtt_username);
  Serial.println(_mqtt_password);
  Serial.println(_mqtt_heartbeat);

  // exit if host is empty
  if (_mqtt_host.isEmpty()) {
    _mqtt_enabled = false;
    return;
  }

  mqttClient.setServer(_mqtt_host.c_str(), _mqtt_port);
  mqttClient.setClientId(_hostname.c_str());
  mqttClient.setKeepAlive(_mqtt_keepalive);
  mqttClient.setCleanSession(false);

  if (_mqtt_will) {
    static char topic_magnitude[48];
    mqttClient.setWill(_mqttTopic("status", topic_magnitude), 1, false, "offline");
  }

  if (!_mqtt_username.isEmpty()) {
    mqttClient.setCredentials(_mqtt_username.c_str(), _mqtt_password.c_str());
  }

  // bump client if hostname or credentials have changed
  if ((reconnect) && mqttClient.connected()) {
    mqttClient.disconnect();
  }
}

bool MQTTClient::connected() {
  return mqttClient.connected();
}

void MQTTClient::loop() {
  if (!_mqtt_enabled) {
    return;  // MQTT not enabled
  }

  _heartbeatCheck();

  // Do not connect if already connected or still trying to connect
  if (mqttClient.connected() || _mqtt_connecting || (WiFi.status() != WL_CONNECTED)) {
    return;
  }

  // Check reconnect interval
  if (millis() - _mqtt_last_connection < _mqtt_reconnect_delay) {
    return;
  }

  _mqtt_connecting = true;  // we're doing a connection

  // Increase the reconnect delay
  _mqtt_reconnect_delay += MQTT_RECONNECT_DELAY_STEP;
  if (_mqtt_reconnect_delay > MQTT_RECONNECT_DELAY_MAX) {
    _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MAX;
  }

  // Connect to the MQTT broker
  Serial.println(F("Connecting to MQTT..."));
  mqttClient.connect();
}

// received MQTT message, pass on to the callback function
// Important to parse are the event strings such as MQTT_MESSAGE_EVENT and MQTT_CONNECT_EVENT
void MQTTClient::onMessage(char* topic, char* payload, size_t len) {
  Serial.println(F("MQTT::onMessage"));
  if (len == 0)
    return;

  Serial.println(F("MQTT::onMessage 1"));
  char message[len + 1];
  strlcpy(message, (char*)payload, len + 1);

  Serial.print(F("MQTT::Received"));
  Serial.print(F("topic : "));
  Serial.println(topic);
  Serial.print(F("message"));
  Serial.println(message);
  // LOGGER.debug(F("Received %s => %s"), topic, message);

  // strip topic
  char* topic_magnitude = strrchr(topic, '/');  // strip out everything until last /
  if (topic_magnitude != nullptr) {
    topic = topic_magnitude + 1;
  }

  // Send message event to custom service
  if (_mqtt_callback_f) {
    (_mqtt_callback_f)(MQTT_MESSAGE_EVENT, topic, message);
  }
}

// MQTT subscribe
// returns false if failed
bool MQTTClient::subscribe(const char* topic) {
  if (mqttClient.connected() && (strlen(topic) > 0)) {
    char topic_magnitude[48];
    char buffer[100];

    uint16_t packet_id = mqttClient.subscribe(_mqttTopic(topic, topic_magnitude), _mqtt_qos);
    if (packet_id) {
      // LOGGER.debug(F("Subscribed to %s"), topic_magnitude);
      sprintf(buffer, "Subscribed to %s", topic_magnitude);
      Serial.println(buffer);
    } else {
      sprintf(buffer, "Error subscribing to %s, error %d", topic_magnitude, packet_id);
      Serial.println(buffer);
      return false;
    }
  }

  return true;
}

// MQTT unsubscribe
void MQTTClient::unsubscribe(const char* topic) {
  if (mqttClient.connected() && (strlen(topic) > 0)) {
    char topic_magnitude[48];
    (void)mqttClient.unsubscribe(_mqttTopic(topic, topic_magnitude));
    // LOGGER.debug(F("Unsubscribed to %s"), topic_magnitude);
  }
}

// Publish using the user's custom retain flag
bool MQTTClient::publish(const char* topic, const char* payload) {
  return publish(topic, payload, _mqtt_retain);
}

// MQTT Publish
// returns true if all good
bool MQTTClient::publish(const char* topic, const char* payload, bool retain) {
  if (mqttClient.connected() && (strlen(topic) > 0)) {
    static char topic_magnitude[48];
    uint16_t packet_id = mqttClient.publish(_mqttTopic(topic, topic_magnitude), _mqtt_qos, retain, payload);
    if (!packet_id) {
      // LOGGER.err(F("Error publishing to %s with payload %s [error %d]"), topic_magnitude, payload, packet_id);
      return false;
    }
  }

  return true;
}

// builds magnitude topic name using base and hostname
// fast string append, 600% faster than using std::string functions
char* MQTTClient::_mqttTopic(const char* topic, char* dst) {
  char* p = dst;
  // add base
  if (!_mqtt_base.isEmpty()) {
    for (size_t i = 0; i < (_mqtt_base.length()); i++) {
      *dst++ = _mqtt_base[i];
    }
    *dst++ = '/';  // add slash
  }

  // add hostname
  for (size_t i = 0; i < (_hostname.length()); i++) {
    *dst++ = _hostname[i];
  }
  *dst++ = '/';  // add slash

  // add topic
  for (size_t i = 0; i < (strlen(topic)); i++) {
    *dst++ = topic[i];
  }

  *dst = '\0';  // terminate string

  return p;  // original dest
}

MQTTClient MQTT;

#endif
