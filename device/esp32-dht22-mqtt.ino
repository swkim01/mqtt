#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <string.h>
#include <DHT.h>

unsigned long requestID = 1;
unsigned long next_heartbeat = 0;
unsigned long sample_time = 10000;

#define DHT_SENSOR   4
#define DHT_TYPE     DHT22
DHT dht(DHT_SENSOR, DHT_TYPE);

const int led_pin=5;

#define MQTT_SERVER "<Server IP>"
WiFiClient wifiClient;
void callback(char* topic, byte* payload, unsigned int length);
PubSubClient mqttClient(MQTT_SERVER, 1883, callback, wifiClient);

char * const loopPacket1 PROGMEM = "{ \"temperature\":";
char * const loopPacket2 PROGMEM = ", \"humidity\":";
char * const loopPacket3 PROGMEM = " }";

const char* ssid = "<SSID>";
const char* password = "<PASSWORD>";

char packetBuffer[48];

void mqttConnect() {
    Serial.println(F("Connecting to MQTT Broker..."));
    if (mqttClient.connect("arduinoclient")) {
      Serial.println(F("Connected to MQTT"));
      mqttClient.subscribe("led");   
     } else {
      Serial.println(F("Failed connecting to MQTT"));
    }
}

void ensureConnected() { 
  if (!wifiClient.connected()) {
    // Connect to WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
  
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("WiFi connected");

    mqttConnect();
  } else {
  }
}

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  char message_buff[length+1];
  Serial.print(F("topic:"));
  Serial.println(topic);  
  
  int i = 0;  
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  Serial.print(F("payload:"));
  String value = String(message_buff);
  Serial.println(value);
  if (value == "ON") {
    digitalWrite(led_pin,HIGH);
  }
  else if (value == "OFF") {
    digitalWrite(led_pin,LOW);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\nStarting...");
  while(!Serial) { }

  Serial.println("Initializing DHT sensor.");
  dht.begin();
  pinMode(led_pin, OUTPUT);

  next_heartbeat = millis() + sample_time;
}

void loop() {
  unsigned long now;
  ensureConnected();
  mqttClient.loop();
  
  now = millis();
  if (now < next_heartbeat) return;
  next_heartbeat = millis() + sample_time;
  readSensor();
  mqttClient.publish("events", packetBuffer);
  requestID = requestID + 1;
}

// read from dht22 sensor
void readSensor()
{
  float humidity, temperature;
  char buffer[24];
  
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  strcpy(packetBuffer, (char*)pgm_read_word(&loopPacket1) );
  if (!isnan(temperature)) {
     strcat(packetBuffer, dtostrf((double) temperature, 4, 2, buffer));
  } else {
    strcat(packetBuffer, dtostrf(0.0, 4, 2, buffer));
  }
  strcat(packetBuffer, (char*)pgm_read_word(&loopPacket2) );
  if (!isnan(humidity)) {
    strcat(packetBuffer, dtostrf((double) humidity, 4, 2, buffer));
  } else {
    strcat(packetBuffer, dtostrf(0.0, 4, 2, buffer));
  }
  strcat(packetBuffer, (char*)pgm_read_word(&loopPacket3) );
  int n = strlen(packetBuffer);
  Serial.print("writing ");Serial.print(n);Serial.println(" octets");
  Serial.println(packetBuffer);
}

