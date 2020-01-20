#ifndef MQTTSettingsService_h
#define MQTTSettingsService_h

#include <Config.h>
#include <AdminSettingsService.h>

#define MQTT_SETTINGS_SERVICE_PATH "/rest/mqttSettings"
#define MQTT_SETTINGS_FILE "/config/mqttSettings.json"

class MQTTSettingsService : public AdminSettingsService {
 public:
  MQTTSettingsService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager);
  ~MQTTSettingsService();

  void loop();
  void begin();

 protected:
  void readFromJsonObject(JsonObject& root);
  void writeToJsonObject(JsonObject& root);
  void onConfigUpdated();

 private:
  bool _enabled;
  String _host;
  String _base;
  int _port;
  bool _retain;
  int _keepalive;
  int _qos;
  bool _will;
  String _username;
  String _password;
  bool _heartbeat;
};

#endif
