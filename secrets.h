#include <pgmspace.h>
 
#define SECRET
 
const char WIFI_SSID[] = "Nombre de la red";         
const char WIFI_PASSWORD[] = "Contraseña";           
 
#define THINGNAME "ESP8266"
 
int8_t TIME_ZONE = -5; //NYC(USA): -5 UTC
 
const char MQTT_HOST[] = "ruta del AWS";

static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
 
 "CERTIFICADOS GENERADOS POR AWS"

-----END CERTIFICATE-----
)EOF";
 
 
// Copy contents from XXXXXXXX-certificate.pem.crt here ▼
static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
 
  "CERTIFICADOS GENERADOS POR AWS"
 
-----END CERTIFICATE-----

 
)KEY";
 
 
// Copy contents from  XXXXXXXX-private.pem.key here ▼
static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
 
  "CERTIFICADOS GENERADOS POR AWS"
 
-----END RSA PRIVATE KEY-----

 
)KEY";
