#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

Servo myservo; 
//-------- Customise these values -----------
const char* ssid = "MLRITM-PLACEMENT";
const char* password = "Mlritm@123";
String command;
#include "DHT.h"
#define DHTPIN D2    // what pin we're connected to
#define DHTTYPE DHT11   // define type of sensor DHT 11
DHT dht (DHTPIN, DHTTYPE);
 
#define ORG "qpbwcx"
#define DEVICE_TYPE "shruthi"
#define DEVICE_ID "sowmya"
#define TOKEN "sarisowmya2000"
int pos;
//-------- Customise the above values --------
 
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/Data/fmt/json";
char topic1[] = "iot-2/cmd/home/fmt/String";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
 
WiFiClient wifiClient;
void callback(char* topic1, byte* payload, unsigned int payloadLength);
PubSubClient client(server,1883,callback,wifiClient);

void setup(){
 Serial.begin(9600);
 Serial.begin(115200);
 Serial.println();
 pinMode(D1,OUTPUT);
 pinMode(D4,OUTPUT);
 pinMode(A0,INPUT);
 dht.begin();
 Serial.print("Connecting to ");
 Serial.print(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 } 
 Serial.println("");
 myservo.attach(D3);
 Serial.print("WiFi connected, IP address: ");
 Serial.println(WiFi.localIP());
}
 
void loop() {
int ldr=analogRead(A0);
Serial.print("LDR value");
Serial.println(ldr);
int mv=map(ldr,0,1024,0,255);
if (ldr>1000)
{
  digitalWrite(D4,LOW);
  Serial.println("LIGHT is Switched OFF");
}
if (ldr<1000)
{
  digitalWrite(D4,HIGH);
  Serial.println("LIGHT is Switched ON");
}
float h = dht.readHumidity();
float t = dht.readTemperature();

if (isnan(h) || isnan(t))
{
Serial.println("Failed to read from DHT sensor!");
delay(1000);
return;
}
PublishData(t,h);
 if (!client.loop()) {
    mqttConnect();
  }
delay(100);
}
void mqttConnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();
    Serial.println();
  }
}
void initManagedDevice() {
  if (client.subscribe(topic1)) {
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* topic1, byte* payload, unsigned int payloadLength) {
  Serial.print("callback invoked for topic1: "); Serial.println(topic1);

  for (int i = 0; i < payloadLength; i++) {
    //Serial.println((char)payload[i]);
    command += (char)payload[i];
  }
Serial.println(command);
if(command == "FANON"){
  digitalWrite(D1,HIGH);
  Serial.println("FAN is Switched ON");
}
if(command == "FANOFF"){
  digitalWrite(D1,LOW);
  Serial.println("FAN is Switched OFF");
}
if(command == "MOTORON"){
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  Serial.println("MOTOR is Switched ON");
}
if(command == "MOTOROFF"){
  myservo.write(pos=0);
  Serial.println("MOTOR is Switched OFF");
}
command ="";
}
void PublishData(float temp, float humid){
 if (!!!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"temperature\":";
  payload += temp;
  payload+="," "\"humidity\":";
  payload += humid;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}
