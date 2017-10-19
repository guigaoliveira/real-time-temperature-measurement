#include <ESP8266WiFi.h>
#include <PubSubClient.h> /* instalar no atom */

#define PORT 1883

const char* ssid = "LII";
const char* password = "wifiLI2Rn";
const char* mqtt_server = "192.168.0.110";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      digitalWrite(D4, LOW);
      Serial.println("Connected!");
    } else {
      digitalWrite(D4, HIGH);
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(D4, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, PORT);
  client.setCallback(callback);
}

long lastMsg = 0;
char buff[8];

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 300) {
    lastMsg = now;
    int analogValue = analogRead(A0);
    float millivolts = (analogValue/1024.0) * 3300; //3300 is the voltage provided by NodeMCU
    float celsius = millivolts/10;
    String msg = String(celsius);
    msg.toCharArray(buff, msg.length()+1);
    Serial.print("Publish message: ");
    Serial.println(buff);
    client.publish("/temperature", buff);
  }
}
