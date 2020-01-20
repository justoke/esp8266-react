#ifndef DemoProject_h
#define DemoProject_h

#include <AdminSettingsService.h>
#include <EOTASettingsService.h>
#include <ESP8266React.h>
#include <FreeRTOS.h>
#include <MQTTClient.h>

#define BLINK_LED 2
#define MAX_DELAY 1000
#define ECHO_CFG_DELAY 15000

#define DEFAULT_BLINK_SPEED 100
#define DEMO_SETTINGS_FILE "/config/demoSettings.json"
#define DEMO_SETTINGS_PATH "/rest/demoSettings"
#define DEFAULT_SETTING "DEFAULT"

class DemoProject : public AdminSettingsService {
 public:
  DemoProject(AsyncWebServer* server, FS* fs, ESP8266React* esp8266React, EOTASettingsService* otaUpdateService);
  ~DemoProject();

  void loop();

  void onMqttMessage(size_t len, const char* topic, const char* payload);

 private:
  ESP8266React* _esp8266React;
  EOTASettingsService* _otaUpdateService;
  unsigned long _lastBlink = 0;
  unsigned long _lastEcho = 0;
  uint8_t _blinkSpeed = 255;
  String _id = DEFAULT_SETTING;
  String _location = DEFAULT_SETTING;
  bool _isSubscribed = false;

 protected:
  void readFromJsonObject(JsonObject& root);
  void writeToJsonObject(JsonObject& root);
  String getDeviceId();
};

#endif
