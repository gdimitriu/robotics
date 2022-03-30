#include <ESP8266WiFi.h>


/* Set these to your desired credentials. */
const char *ssid = "Raspberry-4wd";
const char *password = "12345678";
WiFiServer server(8080);
WiFiClient serverClient;

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.softAP(ssid,password);

  IPAddress myIP = WiFi.softAPIP();
  //start server
  server.begin();
  server.setNoDelay(true);
}

void loop() {
  if (server.hasClient()) {
    serverClient = server.available();
  }
  while (serverClient.available() && Serial.availableForWrite() > 0) {
      Serial.write(serverClient.read());
  }
  while (serverClient.availableForWrite() > 0 && Serial.available() > 0) {
      serverClient.write(Serial.read());
  }
}
