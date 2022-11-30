#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "secrets.h"
#include "DHT.h"
#include <SFE_BMP180.h>
#include <Wire.h>
 
#define DHTPIN 0        // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
#define pinLDR A0       // LDR 

SFE_BMP180 bmp180;      // Object BMP180
 
DHT dht(DHTPIN, DHTTYPE);

char status; 
float h;
float light = 0;
unsigned long lastMillis = 0;
unsigned long previousMillis = 0;
const long interval = 5000;
double Pres=1013.25;
double T,P,A;
int LDR = 0;
 
#define AWS_IOT_PUBLISH_TOPIC   "esp8266/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp8266/sub"
 
WiFiClientSecure net;
 
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);
 
PubSubClient client(net);
 
time_t now;
time_t nowish = 1510592825;
 
 
void NTPConnect(void)
{
  Serial.print("Configuración de la hora mediante SNTP");
  configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("Finalizado!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Tiempo actual: ");
  Serial.print(asctime(&timeinfo));
}
 
 
void messageReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
 
 
void connectAWS()
{
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println(String("Tratando de conectar a la red: ") + String(WIFI_SSID));
 
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
 
  NTPConnect();
 
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
 
  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);
 
 
  Serial.println("Conectando con AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
  }
 
  if (!client.connected()) {
    Serial.println("Tiempo limite superado AWS IoT !");
    return;
  }
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("Conectado exitosamente a AWS IoT!");
}
 
 
void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["Time"] = millis();
  doc["Humidity"] = h;
  doc["Temperature"] = T;
  doc["Pressure"] = P;
  doc["Altitude"] = A;
  doc["Light"] = light;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
 
 
void setup()
{
  Serial.begin(115200);
  connectAWS();
  dht.begin();
  bmp180.begin();
}
 
 
void loop()
{

   
  h = dht.readHumidity();
  LDR=analogRead(pinLDR);
  light=-0.0977*LDR+100;
  
  
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("% , "));
  Serial.print(F("Light: "));
  Serial.print(light);
  Serial.print(F("% , "));
  
  status = bmp180.startTemperature();//Inicio de lectura de temperatura
  if (status != 0)
  {   
    delay(status); //Pausa para que finalice la lectura
    status = bmp180.getTemperature(T); //Obtener la temperatura
    if (status != 0)
    {
      status = bmp180.startPressure(3);//Inicio lectura de presión
      if (status != 0)
      {        
        delay(status);//Pausa para que finalice la lectura        
        status = bmp180.getPressure(P,T);//Obtenemos la presión
        if (status != 0)
        {                  
          Serial.print("Temperature: ");
          Serial.print(T);
          Serial.print(" *C , ");
          Serial.print("Pressure: ");
          Serial.print(P);
          Serial.print(" mb , ");     
          A= bmp180.altitude(P,Pres);
          Serial.print("Altitude: ");
          Serial.print(A);
          Serial.println(" m s.n.m.");    
        }      
      }      
    }   
  }
  
  delay(1000);
 
  now = time(nullptr);
 
  if (!client.connected())
  {
    connectAWS();
  }
  else
  {
    client.loop();
    if (millis() - lastMillis > 3000)
    {
      lastMillis = millis();
      publishMessage();
    }
  }
}
