// This #include statement was automatically added by the Spark IDE.
#include "idDHT22/idDHT22.h"
#include "MQTT/MQTT.h"

// Update these with values suitable for your network.
byte ip[] = { 192, 168, 0, 31 };

int LED = D7; // for demo only

int idDHT22pin = D0; //Digital pin for comunications
void dht22_wrapper(); // must be declared before the lib initialization

// DHT instantiate
idDHT22 DHT22(idDHT22pin, dht22_wrapper);

double humidity = 0;
double temperature = 0;
int ms = 0;
void callback(char*, byte*, unsigned int);

MQTT client(ip, 1883, callback);

void callback(char* topic, byte* payload, unsigned int length) {
    // handle message arrived - we are only subscribing to one topic so assume all are led related
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    
    //client.publish("Spark Status",p);
    if (!strcmp(p,"ON"))
        digitalWrite(LED, HIGH);
    else if (!strcmp(p,"OFF"))
        digitalWrite(LED, LOW);
}

// Simple MQTT demo to allow the blue led (D7) to be turned on or off. Send message to topic "led" with payload of "on" or "off"

void setup()
{
    pinMode(LED, OUTPUT); // Use for a simple test of the led on or off by subscribing to a topical called led
    
    if (client.connect("spark")) { // Anonymous authentication enabled
    //if (client.connect("spark", "userid", "password")) { // uid:pwd based authentication
        client.publish("Spark Status","I'm Alive...");
        client.subscribe("led");
    }
}

void dht22_wrapper() {
	DHT22.isrCallback();
}

void loop()
{
    char message[48];
    
    client.loop();
    
    if (millis() - ms > 10000) {
        //client.publish("Spark Status","events");
        DHT22.acquire();
        while (DHT22.acquiring()) 
            ;
        temperature = DHT22.getCelsius();
        humidity = DHT22.getHumidity();
        sprintf(message, "{ \"temperature\":%.2f, \"humidity\":%.2f }", temperature, humidity);
        client.publish("events", message);
        ms = millis();
    }
}