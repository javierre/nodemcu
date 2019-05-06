#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>


const char *ssid_AP = "ESP8266_AP"; 
const char *password_AP = "12345678";

const char *ssid_STA = "MyEXTERNALssid"; 
const char *password_STA = "mypassword"; 
ESP8266WebServer server(80);

void setup() { 
  Serial.begin(115200);
  delay(10); 
  Serial.println();

  WiFi.mode(WIFI_AP_STA); 
  WiFi.softAP(ssid_AP, password_AP); // Para la red generada por nodemcu
  WiFi.begin(ssid_STA, password_STA);// Para conectarse a la WiFi externa
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); 
    Serial.print(".");
  }

  WiFi.setAutoReconnect(true); 
  Serial.println("WiFi conectada.");

  Serial.println(); 
  WiFi.printDiag(Serial); 
  Serial.println();
  
  Serial.print("AP dirección IP: "); 
  Serial.println(WiFi.softAPIP()); 
  Serial.print("STA dirección IP: "); 
  Serial.println(WiFi.localIP());

  server.on("/", [](){
    server.send(200, "text/plain", "Hola Mundo!!");
  });
  server.begin(); 
  Serial.println("Servidor inicializado.");
}

void loop() { 
  server.handleClient();
}


