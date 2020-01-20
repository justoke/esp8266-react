#ifndef EOTASettingsService_h
#define EOTASettingsService_h

#include <AdminSettingsService.h>
#include <EOTAUpdate.h>

#ifdef ESP32
#include <ESPmDNS.h>
#elif defined(ESP8266)
#include <ESP8266mDNS.h>
#endif

//#include <ArduinoOTA.h>
#include <WiFiUdp.h>

// Emergency defaults
#define DEFAULT_OTA_PORT 8266
#define DEFAULT_OTA_PASSWORD "esp-react"

#define EOTA_SETTINGS_FILE "/config/otaSettings.json"
#define EOTA_SETTINGS_SERVICE_PATH "/rest/otaSettings"

class EOTASettingsService : public AdminSettingsService {
 public:
  EOTASettingsService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager);
  ~EOTASettingsService();

  void loop();

 protected:
  void onConfigUpdated();
  void readFromJsonObject(JsonObject& root);
  void writeToJsonObject(JsonObject& root);

 private:
  EOTAUpdate* _updater;
  bool _enabled;
  int _port;
  String _password;
  String _updateurl;

  void configureOTA();
#ifdef ESP32
  void onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
#elif defined(ESP8266)
  WiFiEventHandler _onStationModeGotIPHandler;
  void onStationModeGotIP(const WiFiEventStationModeGotIP& event);
#endif
};

#endif  // end EOTASettingsService_h
