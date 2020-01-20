#include "services.h"

#if MQTT_SUPPORT

#ifndef MQTTClient_h
#define MQTTClient_h

#include <AsyncJson.h>
#include <AsyncMqttClient.h>
#include <MQTTSettingsService.h>

#define MQTT_RECONNECT_DELAY_MIN 2000    // Try to reconnect in 3 seconds upon disconnection
#define MQTT_RECONNECT_DELAY_STEP 3000   // Increase the reconnect delay in 3 seconds after each failed attempt
#define MQTT_RECONNECT_DELAY_MAX 120000  // Set reconnect time to 2 minutes at most

#define MQTT_HEARTBEAT_INTERVAL 120000  // in milliseconds, how often the MQTT heartbeat is sent (2 mins)

// Internal MQTT events
#define MQTT_CONNECT_EVENT 0
#define MQTT_DISCONNECT_EVENT 1
#define MQTT_MESSAGE_EVENT 2

// defaults - use with e.g. uuid::read_flash_string(F_(mqtt_will_topic)).c_str()
// MAKE_PSTR_WORD(mqtt)
// MAKE_PSTR(mqtt_will_topic, "status")
// MAKE_PSTR(mqtt_will_payload_online, "online")
// MAKE_PSTR(mqtt_will_payload_offline, "offline")
// MAKE_PSTR(mqtt_will_topic_heartbeat, "heartbeat")

class MQTTClient {
 public:
  void setup(MQTTSettingsService* mqttSettingsService);
  void loop();
  void configure(bool reconnect);

  bool subscribe(const char* topic);
  void unsubscribe(const char* topic);
  bool publish(const char* topic, const char* payload, bool retain);
  bool publish(const char* topic, const char* payload);
  bool connected();

  // void print_status(uuid::console::Shell& shell);

  typedef std::function<void(const unsigned int, const char*, const char*)> mqtt_callback_f;
  void setMQTTcallback(mqtt_callback_f callback);

 protected:
 private:
  AsyncMqttClient mqttClient;  // the MQTT class library

  void onConnect();
  void onMessage(char* topic, char* payload, size_t len);
  mqtt_callback_f _mqtt_callback_f;  // custom callback

  bool _mqtt_connecting;
  uint32_t _mqtt_reconnect_delay;
  uint32_t _mqtt_last_connection;

  // for storing config settings
  bool _mqtt_enabled;
  String _mqtt_host;
  String _mqtt_base;
  String _hostname;
  int _mqtt_qos = 0;
  int _mqtt_port = 1883;
  bool _mqtt_retain = false;
  bool _mqtt_heartbeat = false;
  bool _mqtt_will = false;
  int _mqtt_keepalive = 60;
  String _mqtt_username = "";
  String _mqtt_password = "";

  char* _mqttTopic(const char* topic, char* dst);
  void _heartbeatCheck(bool force = false);
};

extern MQTTClient MQTT;

#endif

#endif
