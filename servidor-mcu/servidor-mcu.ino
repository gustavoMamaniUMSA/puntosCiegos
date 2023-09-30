#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define pin_trig_i D0
#define pin_echo_i D1

#define pin_trig_d D7
#define pin_echo_d D8

#define pin_trig_a D4
#define pin_echo_a D5


// #define pin_izquierda D6
// #define pin_inactivo D7
// #define pin_derecha D8

#define pin_direccional A0

#ifndef APSSID
#define APSSID "puntosCiegos"
#define APPSK  "visorPCiegos"
#endif

const char *ssid = APSSID;
const char *password = APPSK;

int estadoIzquierda = 0;
int estadoInactivo = 1;
int estadoDerecha = 0;

int intervaloIzquierda = 200;
int intervaloInactivo = 622;
int intervaloDerecha = 930;
int tolerancia = 100;

unsigned long interval = 25;
unsigned long previousMillis = 0;
int DELAY_RECONNECT = 3000;

bool primeraConexion = true;

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
  // pinMode(pin_izquierda, INPUT);
  // pinMode(pin_inactivo, INPUT);
  // pinMode(pin_derecha, INPUT);
  pinMode(pin_direccional, INPUT);
  Serial.println();
  Serial.print("Configurando red wifi...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password,1,false,max_connection);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

// Lectura de distancias (retorna el valor en centímetros)
float lecturaDistancia(uint8_t pin_trig, uint8_t pin_echo) {
  unsigned long tiempo;
  float distancia;
  digitalWrite(pin_trig, LOW);
  delayMicroseconds(4);
  digitalWrite(pin_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin_trig, LOW);
  tiempo = pulseIn(pin_echo, HIGH);
  if (tiempo > 24000) {
    return 0.0;
  }
  distancia = tiempo / 58.3;
  delay(1);
  return distancia;
}

// Lectura de distancias (retorna el valor en centímetros)
float lecturaIzquierda() {
  float tiempo;
  float distancia;
  digitalWrite(pin_trig_i, LOW);
  delayMicroseconds(4);
  digitalWrite(pin_trig_i, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin_trig_i, LOW);
  tiempo = pulseIn(pin_echo_i, HIGH);
  distancia = tiempo / 58.3;
  delay(1);
  return distancia;
}

// Lectura de distancias (retorna el valor en centímetros)
float lecturaDerecha() {
  float tiempo;
  float distancia;
  digitalWrite(pin_trig_d, LOW);
  delayMicroseconds(4);
  digitalWrite(pin_trig_d, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin_trig_d, LOW);
  tiempo = pulseIn(pin_echo_d, HIGH);
  distancia = tiempo / 58.3;
  delay(1);
  return distancia;
}

// Lectura de distancias (retorna el valor en centímetros)
float lecturaAtras() {
  float tiempo;
  float distancia;
  digitalWrite(pin_trig_a, LOW);
  delayMicroseconds(4);
  digitalWrite(pin_trig_a, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin_trig_a, LOW);
  tiempo = pulseIn(pin_echo_a, HIGH);
  distancia = tiempo / 58.3;
  delay(1);
  return distancia;
}

String lecturaAnalogica() {
  String lectura = "";

  int primeraLectura = analogRead(pin_direccional);
  delayMicroseconds(50);
  int segundaLectura = analogRead(pin_direccional);

  // Guiñador izquierdo
  if (primeraLectura > (intervaloIzquierda - tolerancia) && primeraLectura < (intervaloIzquierda + tolerancia)) {
    if (segundaLectura > (intervaloIzquierda - tolerancia) && segundaLectura < (intervaloIzquierda + tolerancia)) {
      estadoIzquierda = 1;
      estadoInactivo = 0;
      estadoDerecha = 0;
    }
  }

  // Guiñador inactivado
  if (primeraLectura > (intervaloInactivo - tolerancia) && primeraLectura < (intervaloInactivo + tolerancia)) {
    if (segundaLectura > (intervaloInactivo - tolerancia) && segundaLectura < (intervaloInactivo + tolerancia)) {
      estadoIzquierda = 0;
      estadoInactivo = 1;
      estadoDerecha = 0;
    }
  }

  // giñador derecho
  if (primeraLectura > (intervaloDerecha - tolerancia) && primeraLectura < (intervaloDerecha + tolerancia)) {
    if (segundaLectura > (intervaloDerecha - tolerancia) && segundaLectura < (intervaloDerecha + tolerancia)) {
      estadoIzquierda = 0;
      estadoInactivo = 0;
      estadoDerecha = 1;
    }
  }

  lectura = ";" + String(estadoIzquierda)+";"+String(estadoInactivo)+";"+String(estadoDerecha)+";";
  return lectura;
}

void loop() {
  while(!client.connect(serverIP, serverPort)) {
    Serial.println("Estableciendo conexión con el servidor");
    delay(DELAY_RECONNECT);
  }

  if (primeraConexion) {
    Serial.println("Conexión establecida con éxito");
    primeraConexion = false;
  }

  String lecturas = lecturaAnalogica();
  // float resultado;
  unsigned long currentMillis = millis();
  if(((currentMillis - previousMillis) >= interval) && (client.connect(serverIP, serverPort))){
    previousMillis = currentMillis;

    // Lectura de distancia de sensor izquierdo
    // resultado = lecturaIzquierda();
    float resultado_i = lecturaDistancia(pin_trig_i, pin_echo_i);
    lecturas += String(resultado_i);

    // Lectura de distancia de sensor trasero
    // resultado = lecturaAtras();
    float resultado_a = lecturaDistancia(pin_trig_a, pin_echo_a);
    lecturas += ";"+String(resultado_a);

    // Lectura de distancia de sensor derecho
    // resultado = lecturaDerecha();
    float resultado_d = lecturaDistancia(pin_trig_d, pin_echo_d);
    lecturas += ";"+String(resultado_d);

    Serial.println(lecturas);
    client.print(String(lecturas)+"\n");
    delay(1);
    client.stop();
  }
}

