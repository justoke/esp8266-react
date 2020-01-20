#include <DemoProject.h>
#include <ESP8266React.h>
#include <FS.h>

void AdvertiseServices();

#define SERIAL_BAUD_RATE 115200

AsyncWebServer server(80);
ESP8266React esp8266React(&server, &SPIFFS);
EOTASettingsService otaUpdateService(&server, &SPIFFS, esp8266React.getSecurityManager());
// DemoProject demoProject = DemoProject(&server, &SPIFFS, esp8266React.getSecurityManager(), &otaUpdateService);
DemoProject demoProject = DemoProject(&server, &SPIFFS, &esp8266React, &otaUpdateService);

void setup() {
  // start serial and filesystem
  Serial.begin(SERIAL_BAUD_RATE);

  // start the file system (must be done before starting the framework)
#ifdef ESP32
  SPIFFS.begin(true);
#elif defined(ESP8266)
  SPIFFS.begin();
#endif

  // config_load_settings();
  // start the framework and demo project
  esp8266React.begin();

  otaUpdateService.begin();

  // start the demo project
  demoProject.begin();

  AdvertiseServices();
  // start the server
  server.begin();
}

void loop() {
  // run the framework's loop function
  esp8266React.loop();

  // run the demo project's loop function
  demoProject.loop();
}

void AdvertiseServices() {
  String MyName = "Quantum_Controller";
  if (MDNS.begin(MyName.c_str())) {
    Serial.println(F("mDNS responder started"));
    Serial.print(F("My name is: "));
    Serial.println(MyName.c_str());

    // Add service to MDNS-SD
    MDNS.addService("QSMCU-12345", "tcp", 23);
  } else {
    while (1) {
      Serial.println(F("Error setting up MDNS responder"));
      delay(1000);
    }
  }
}
