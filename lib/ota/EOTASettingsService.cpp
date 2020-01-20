#include <EOTASettingsService.h>

//#define FIRMWARE_VERSION_STRING "6.0"
//#define FIRMWARE_VERSION_NUMBER 6

const char* const VERSION_STRING = String(FIRMWARE_VERSION_STRING).c_str();
const unsigned short VERSION_NUMBER = FIRMWARE_VERSION_NUMBER;
EOTASettingsService::EOTASettingsService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager) :
    AdminSettingsService(server, fs, securityManager, EOTA_SETTINGS_SERVICE_PATH, EOTA_SETTINGS_FILE) {
#ifdef ESP32
  WiFi.onEvent(std::bind(&EOTASettingsService::onStationModeGotIP, this, std::placeholders::_1, std::placeholders::_2),
               WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
#elif defined(ESP8266)
  _onStationModeGotIPHandler =
      WiFi.onStationModeGotIP(std::bind(&EOTASettingsService::onStationModeGotIP, this, std::placeholders::_1));
#endif
}

EOTASettingsService::~EOTASettingsService() {
}

void EOTASettingsService::loop() {
  if (_enabled) {
    if (_updater) {
      _enabled = false;

      if (_updater->CheckAndUpdate()) {
        log_i("Checking for updates. Remember to set the URL!");

        Serial.println(_updateurl);
      }
      _enabled = true;
    }
  }
}

void EOTASettingsService::onConfigUpdated() {
  configureOTA();
}

void EOTASettingsService::readFromJsonObject(JsonObject& root) {
  _enabled = root["enabled"];
  _port = root["port"];
  _updateurl = root["updateurl"].as<String>();
  _password = root["password"] | DEFAULT_OTA_PASSWORD;

  // provide defaults
  if (_port < 0) {
    _port = DEFAULT_OTA_PORT;
  }
}

void EOTASettingsService::writeToJsonObject(JsonObject& root) {
  root["enabled"] = _enabled;
  root["port"] = _port;
  root["password"] = _password;
  root["updateurl"] = _updateurl;
}

void EOTASettingsService::configureOTA() {
  if (_updater) {
    delete _updater;
    _updater = nullptr;
  }

  if (_enabled) {
    log_i("Current Version : ");
    Serial.print(String(FIRMWARE_VERSION_STRING));

    log_i("_updateurl : ");
    Serial.print(_updateurl);
#ifdef OTA_UPDATE_URL
    log_i("OTA_UPDATE_URL : ");
    Serial.print(OTA_UPDATE_URL);
    log_i("Using url from ini file");
    _updater = new EOTAUpdate(OTA_UPDATE_URL, VERSION_NUMBER, 3600UL);
#else
    log_i("Using _updateurl from otaSettings.json file");
    _updater = new EOTAUpdate(_updateurl, VERSION_NUMBER, 3600UL);
#endif
  }
}
#ifdef ESP32
void EOTASettingsService::onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  configureOTA();
}
#elif defined(ESP8266)
void EOTASettingsService::onStationModeGotIP(const WiFiEventStationModeGotIP& event) {
  configureOTA();
}
#endif
