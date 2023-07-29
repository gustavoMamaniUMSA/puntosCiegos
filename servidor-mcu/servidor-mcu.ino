#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define pin_trig_i D0
#define pin_echo_i D1

#define pin_trig_a D2
#define pin_echo_a D3

#define pin_trig_d D4
#define pin_echo_d D5

#define pin_izquierda D6
#define pin_inactivo D7
#define pin_derecha D8

#ifndef APSSID
#define APSSID "puntosCiegos"
#define APPSK  "visorPCiegos"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

int estadoIzquierda = 0;
int estadoInactivo = 1;
int estadoDerecha = 0;
unsigned long startTime;
unsigned long interval = 100;

const IPAddress serverIP(192,168,4,2); // dirección IP del servidor
uint16_t serverPort = 9700;         // Número de puerto del servidor
uint8_t max_connection = 1;

WiFiClient client;

void setup() {
  delay(1000);
  Serial.begin(115200);
  pinMode(pin_trig_i, OUTPUT);
  pinMode(pin_echo_i, INPUT);
  pinMode(pin_trig_a, OUTPUT);
  pinMode(pin_echo_a, INPUT);
  pinMode(pin_trig_d, OUTPUT);
  pinMode(pin_echo_d, INPUT);
  pinMode(pin_izquierda, INPUT);
  pinMode(pin_inactivo, INPUT);
  pinMode(pin_derecha, INPUT);
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password,1,false,max_connection);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  //server.begin();
  startTime = millis();
}

float senIzquierda(){
  float tiempo;
  float distancia;
  digitalWrite(pin_trig_i, LOW);
  delayMicroseconds(4);
  digitalWrite(pin_trig_i, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin_trig_i, LOW);
  tiempo = pulseIn(pin_echo_i, HIGH);
  distancia = tiempo/58.3;
  delay(1);
  return distancia;
}

float senDerecha(){
  float tiempo;
  float distancia;
  digitalWrite(pin_trig_d, LOW);
  delayMicroseconds(4);
  digitalWrite(pin_trig_d, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin_trig_d, LOW);
  tiempo = pulseIn(pin_echo_d, HIGH);
  distancia = tiempo/58.3;
  delay(1);
  return distancia;
}

float senAtras(){
  float tiempo;
  float distancia;
  digitalWrite(pin_trig_a, LOW);
  delayMicroseconds(4);
  digitalWrite(pin_trig_a, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin_trig_a, LOW);
  tiempo = pulseIn(pin_echo_a, HIGH);
  distancia = tiempo/58.3;
  
  delay(1);
  return distancia;
}

String estadoLectura(){
  String lectura = "";
  if(digitalRead(pin_izquierda)==1){
    delayMicroseconds(100);
    if(digitalRead(pin_izquierda)==1){
      estadoIzquierda = 1;
      estadoInactivo = 0;
      estadoDerecha = 0;
    }
  }
  if(digitalRead(pin_inactivo)==1){
    delayMicroseconds(100);
    if(digitalRead(pin_inactivo)==1){
      estadoIzquierda = 0;
      estadoInactivo = 1;
      estadoDerecha = 0;
    }
  }
  if(digitalRead(pin_derecha)==1){
    delayMicroseconds(100);
    if(digitalRead(pin_derecha)==1){  
      estadoIzquierda = 0;
      estadoInactivo = 0;
      estadoDerecha = 1;
    }
  }
  lectura = ";" + String(estadoIzquierda)+";"+String(estadoInactivo)+";"+String(estadoDerecha)+";";
  return lectura;
}

void loop() {
  String lecturas = estadoLectura();
  float resultado;
  if((millis() - startTime) >= interval){
    Serial.println("Intenta acceder al servidor");
    if(client.connect(serverIP, serverPort)){
      Serial.println("Visita exitosa");
      resultado = senIzquierda();
      lecturas += String(resultado);
      resultado = senAtras();
      lecturas += ";"+String(resultado);
      resultado = senDerecha();
      lecturas += ";"+String(resultado);
      //client.print(String("hola.")+"\n");
      Serial.println(lecturas);
      client.print(String(lecturas)+"\n");
      delay(1);
    }else{
      Serial.println("Acceso fallido");
    }
    client.stop();
    startTime = millis();
  }
}
