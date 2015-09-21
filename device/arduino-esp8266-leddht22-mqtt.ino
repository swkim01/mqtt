#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <ESP8266.h>
#include <ESP8266Client.h>
#include <string.h>
#include <DHT.h>

unsigned long requestID = 1;
unsigned long next_heartbeat = 0;
unsigned long sample_time = 10000;

#define DHT_SENSOR   A2
#define DHT_TYPE     DHT22
DHT dht(DHT_SENSOR, DHT_TYPE);

const int led_pin=5;

#define MQTT_SERVER "<server ip>"

SoftwareSerial esp8266Serial = SoftwareSerial(2, 3);
ESP8266 wifi = ESP8266(esp8266Serial);
ESP8266Client wifiClient(wifi, ESP8266_SINGLE_CLIENT);
PubSubClient mqttClient(MQTT_SERVER, 1883, callback, wifiClient);

char * const loopPacket1 PROGMEM = "{ \"temperature\":";
char * const loopPacket2 PROGMEM = ", \"humidity\":";
char * const loopPacket3 PROGMEM = " }";

#define WLAN_SSID       "<SSID>"   // cannot be longer than 32 characters!
#define WLAN_PASS       "<PASSWORD>"
#define WLAN_SECURITY   WLAN_SEC_WPA2

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
    wifi.begin();
  
    // setWifiMode
    Serial.print("setWifiMode: ");
    Serial.println(getStatus(wifi.setMode(ESP8266_WIFI_STATION)));
  
    // joinAP
    Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
    Serial.println(getStatus(wifi.joinAP(WLAN_SSID, WLAN_PASS)));
    Serial.println(F("Connected!"));

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
  Serial.begin(9600);
  Serial.println("\nStarting...");
  while(!Serial) { }

  Serial.println("Initializing DHT sensor.");
  dht.begin();
  
  pinMode(led_pin, OUTPUT);

  // ESP8266
  Serial.println(F("\nInitializing..."));
  esp8266Serial.begin(9600);
  
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

  strcpy(packetBuffer,(char*)pgm_read_word(&loopPacket1) );
  if (!isnan(temperature)) {
     strcat(packetBuffer, dtostrf((double) temperature, 4, 2, buffer));
  } else {
    strcat(packetBuffer, dtostrf(0.0, 4, 2, buffer));
  }
  strcat(packetBuffer,(char*)pgm_read_word(&loopPacket2) );
  if (!isnan(humidity)) {
    strcat(packetBuffer, dtostrf((double) humidity, 4, 2, buffer));
  } else {
    strcat(packetBuffer, dtostrf(0.0, 4, 2, buffer));
  }
  strcat(packetBuffer,(char*)pgm_read_word(&loopPacket3) );

  int n = strlen(packetBuffer);
  Serial.print("writing ");Serial.print(n);Serial.println(" octets");
  Serial.println(packetBuffer);
}

String getStatus(bool status)
{
  if (status)
    return "OK";
  return "KO";
}

String getStatus(ESP8266CommandStatus status)
{
  switch (status) {
  case ESP8266_COMMAND_INVALID:
    return "INVALID";
    break;
  case ESP8266_COMMAND_TIMEOUT:
    return "TIMEOUT";
    break;
  case ESP8266_COMMAND_OK:
    return "OK";
    break;
  case ESP8266_COMMAND_NO_CHANGE:
    return "NO CHANGE";
    break;
  case ESP8266_COMMAND_ERROR:
    return "ERROR";
    break;
  case ESP8266_COMMAND_NO_LINK:
    return "NO LINK";
    break;
  case ESP8266_COMMAND_TOO_LONG:
    return "TOO LONG";
    break;
  case ESP8266_COMMAND_FAIL:
    return "FAIL";
    break;
  default:
    return "UNKNOWN COMMAND STATUS";
    break;
  }
}
