/*
 Board: WeMos D1 MINI (https://tronixlabs.com.au/arduino/boards/wireless/wemos-d1-mini-arduino-compatible-esp8266-wifi-australia/)
 Shield: https://tronixlabs.com.au/esp8266/wemos-esp8266/wemos-d1-mini-relay-shield-v2-australia/
 Libraries: 
  ESP8266Wifi (Add via Additional Boards Link): http://www.instructables.com/id/Wemos-ESP8266-Getting-Started-Guide-Wemos-101/
  PubSubClient (Add via Library Manager): https://github.com/knolleary/pubsubclient

 Whacked together by James Rule and Ethan Dunne for the MakerSpaceIoT 101 Workshop
 */
 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network
const char* ssid = "iAccelerate";
const char* password = "AcceLer47eIC$";
// The MQTT Server you want to use, this is https://test.mosquitto.org/ 
const char* mqtt_server = "test.mosquitto.org"; 

// Variables for the MQTT Library
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const int relayPin = D1; // For the WeMos D1 Mini Relay Shield
const long interval = 500; // The delay ensures the relay cannot be triggered too quickly 
const int ledPin = BUILTIN_LED;

void setup() {
        Serial.begin(115200);
        setup_wifi();
        client.setServer(mqtt_server, 1883);
        pinMode(relayPin, OUTPUT);
        pinMode(ledPin, OUTPUT);  // set initial state, LED OFF
        digitalWrite(ledPin, HIGH); // LED Off
        client.setCallback(callback); // The callback will trigger the relay when a message arrives
}

void setup_wifi() { 
        delay(10);
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
        }
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
}

void reconnect() {
        while (!client.connected()) {
                Serial.print("Attempting MQTT connection...");
                String clientId = "ESP8266Client-";
                clientId += String(random(0xffff), HEX);
                if (client.connect(clientId.c_str())) {
                        Serial.println("connected");
                        client.subscribe("/MSIOT101/OUT");
                } else {
                        Serial.print("failed, rc=");
                        Serial.print(client.state());
                        Serial.println(" try again in 5 seconds");
                        delay(5000);
                }
        }
}

void callback(char* topic, byte* payload, unsigned int length) {
        Serial.print("Message arrived [");
        Serial.print(topic);
        Serial.print("] ");
        for (int i = 0; i < length; i++) {
                Serial.print((char)payload[i]);
        }
        Serial.println();        
        if ((char)payload[0] == '1') {        
                digitalWrite(relayPin, HIGH); // Turn ON relay with voltage HIGH
                digitalWrite(ledPin, LOW); // LED On
                delay(interval);   
        } else {
                digitalWrite(relayPin, LOW);  // Turn OFF relay with voltage LOW
                digitalWrite(ledPin, HIGH); // LED Off
                delay(interval);
        }
}

void loop() {
        if (!client.connected()) {
          reconnect();
        }
        client.loop();
}

