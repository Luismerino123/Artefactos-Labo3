#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID  "ARTEFACTOS"
#define WLAN_PASS  "ARTEFACTOS"

#define AIO_SERVER     "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME   "Luismerino123"
#define AIO_KEY        "key"  

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish luz = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperatura");

Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

const int LDR_PIN = 33;  

void connectWiFi() {
  Serial.begin(115200);
  Serial.println();
  Serial.printf("Conectando a %s\n", WLAN_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print("\nWiFi OK. IP: "); Serial.println(WiFi.localIP());
}

void MQTT_connect() {
  if (mqtt.connected()) return;
  Serial.print("Conectando a MQTT... ");
  int8_t ret;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reintento en 5 s...");
    mqtt.disconnect();
    delay(5000);
  }
  Serial.println("MQTT conectado!");
  mqtt.subscribe(&onoffbutton);
}

void setup() {
  connectWiFi();
}

void loop() {
  MQTT_connect();
  mqtt.processPackets(10000);
  if (!mqtt.ping()) mqtt.disconnect();

  static uint32_t lastPub = 0;
  if (millis() - lastPub > 2000) {
    lastPub = millis();
    int raw = analogRead(LDR_PIN);        
    Serial.printf("LDR=%d\n", raw);
    if (!luz.publish((int32_t)raw)) {      
      Serial.println("Falló publicar");
    }
  }
}