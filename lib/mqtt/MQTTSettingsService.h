#ifndef MQTTSettingsService_h
#define MQTTSettingsService_h

#include <Config.h>
#include <AdminSettingsService.h>

#define MQTT_SETTINGS_SERVICE_PATH "/rest/mqttSettings"
#define MQTT_SETTINGS_FILE "/config/mqttSettings.json"

class MQTTSettings {
 public:
  bool enabled;
  String host;
  String base;
  int port;
  bool retain;
  int keepalive;
  int qos;
  bool will;
  String username;
  String password;
  bool heartbeat;
};

class MQTTSettingsService : public AdminSettingsService<MQTTSettings> {
 public:
  MQTTSettingsService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager);
  ~MQTTSettingsService();

  void loop();
  void begin();

 protected:
  void readFromJsonObject(JsonObject& root);
  void writeToJsonObject(JsonObject& root);
  void onConfigUpdated();
};

#endif
