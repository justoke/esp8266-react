#include <Config.h>
#include <DemoProject.h>
#include <NtpClientLib.h>

/* DemoProject::DemoProject(AsyncWebServer* server,
                         FS* fs,
                         SecurityManager* securityManager,
                         EOTASettingsService* otaUpdateService) :
    AdminSettingsService(server, fs, securityManager, DEMO_SETTINGS_PATH, DEMO_SETTINGS_FILE) */

DemoProject::DemoProject(AsyncWebServer* server,
                         FS* fs,
                         ESP8266React* esp8266React,
                         EOTASettingsService* otaUpdateService) :
    AdminSettingsService(server, fs, esp8266React->getSecurityManager(), DEMO_SETTINGS_PATH, DEMO_SETTINGS_FILE),
    _esp8266React(esp8266React) {
  pinMode(BLINK_LED, OUTPUT);

  _otaUpdateService = otaUpdateService;

  MQTT.setMQTTcallback(std::bind(
      &DemoProject::onMqttMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

DemoProject::~DemoProject() {
}

void DemoProject::onMqttMessage(size_t len, const char* topic, const char* payload) {
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  payload: ");
  Serial.println(payload);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");

  if (String(topic).equalsIgnoreCase(F("OTACheck"))) {
    _otaUpdateService->loop();
  }

  if (String(topic).equalsIgnoreCase(F("otaSettings"))) {
    String config = "";
    _esp8266React->getOTASettingsService()->fetchAsString(config);
    Serial.println(config);
    config = String(payload);
    _esp8266React->getOTASettingsService()->updateFromString(config);
    Serial.println(config);
  }
}

void DemoProject::loop() {
  unsigned delay = MAX_DELAY / 255 * (255 - _settings.blinkSpeed);
  unsigned long currentMillis = millis();
  if (!_lastBlink || (unsigned long)(currentMillis - _lastBlink) >= delay) {
    _lastBlink = currentMillis;
    digitalWrite(BLINK_LED, !digitalRead(BLINK_LED));
  }

  if (MQTT.connected() && !_isSubscribed) {
    _isSubscribed = MQTT.subscribe("OTAUpdate");
    _isSubscribed = MQTT.subscribe("apSettings");
    _isSubscribed = MQTT.subscribe("otaSettings");
    _isSubscribed = MQTT.subscribe("ntpSettings");
    _isSubscribed = MQTT.subscribe("wifiSettings");
    _isSubscribed = MQTT.subscribe("securitySettings");
    _isSubscribed = MQTT.subscribe("#");
  }

  if (!_lastEcho || (unsigned long)(currentMillis - _lastEcho) >= ECHO_CFG_DELAY) {
    _lastEcho = currentMillis;

    // round-trip the settings, just for fun.
    String config;
    _esp8266React->getOTASettingsService()->fetchAsString(config);
    Serial.println(config);
    config = "";
    _esp8266React->getSecuritySettingsService()->fetchAsString(config);
    Serial.println(config);
    config = "";
    _esp8266React->getAPSettingsService()->fetchAsString(config);
    Serial.println(config);
    config = "";
    _esp8266React->getNTPSettingsService()->fetchAsString(config);
    Serial.println(config);
    config = "";
    _esp8266React->getWiFiSettingsService()->fetchAsString(config);
    Serial.println(config);
  }
}

void DemoProject::readFromJsonObject(JsonObject& root) {
  _id = root["id"] | DEFAULT_SETTING;
  _location = root["location"] | DEFAULT_SETTING;
  _blinkSpeed = root["blink_speed"] | DEFAULT_BLINK_SPEED;
  _settings.blinkSpeed = root["blink_speed"] | DEFAULT_BLINK_SPEED;
}

void DemoProject::writeToJsonObject(JsonObject& root) {
  // connection settings
  root["id"] = _id;
  root["location"] = _location;
  root["blink_speed"] = _blinkSpeed;
  root["blink_speed"] = _settings.blinkSpeed;
}
