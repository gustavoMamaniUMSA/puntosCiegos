#include <ESP8266WiFi.h>

const char *ssid = "puntosCiegos";
const char *password = "visorPCiegos";

#define PIN_TRIG D5
#define PIN_ECHO D6
float tiempo;
float distancia;

const IPAddress serverIP(192,168,100,94); // dirección IP del servidor
uint16_t serverPort = 9700;         // Número de puerto del servidor

WiFiClient client; // Declarar un objeto cliente para conectarse al servidor

void setup()
{
    Serial.begin(115200);
    Serial.println();
    
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false); // Desactiva la suspensión de wifi en modo STA para mejorar la velocidad de respuesta
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");
    Serial.print("IP Address:");
    Serial.println(WiFi.localIP());
    pinMode(PIN_TRIG, OUTPUT);
    pinMode(PIN_ECHO, INPUT);
}

void lecturaUltrasonido(){
    digitalWrite(PIN_TRIG, LOW);  
    delayMicroseconds(4);
   
    digitalWrite(PIN_TRIG, HIGH);  
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);
   
    tiempo = pulseIn(PIN_ECHO, HIGH);
    distancia = tiempo/58.3;

    Serial.println(distancia);
}

void loop()
{
    Serial.println("Intenta acceder al servidor");
    if (client.connect(serverIP, serverPort)) // Intenta acceder a la dirección de destino
    {
        Serial.println("Visita exitosa");

        client.print("Hello world!");                    
        while (client.connected() || client.available()) 
        {
            if (client.available()) 
            {
                String line = client.readStringUntil('\n'); 
                Serial.print("Leer datos:");
                Serial.println(line);
                client.write(line.c_str()); 
            }
            lecturaUltrasonido()
            String lectura = String(distancia,2);
            client.write(lectura.c_str());
        }
        Serial.println("Cerrar la conexión actual");
        client.stop(); // Cerrar el cliente
    }
    else
    {
        Serial.println("Acceso fallido");
        client.stop(); // Cerrar el cliente
    }
    delay(5000);
}
