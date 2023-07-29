#define PIN_TRIG D5
#define PIN_ECHO D6

#define pin_trig D3
#define pin_echo D4

float tiempo;
float distancia;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(pin_trig, OUTPUT);
  pinMode(pin_echo, INPUT);
}
void senIzquierda(){
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(4);

  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  tiempo = pulseIn(PIN_ECHO, HIGH);
  distancia = tiempo/58.3;
  Serial.print("Izquierda tiempo: ");
  Serial.print(tiempo);
  Serial.print("   distancia: ");
  Serial.println(distancia);

  delay(1000);
}

void senDerecha(){
  digitalWrite(pin_trig, LOW);
  delayMicroseconds(4);

  digitalWrite(pin_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  tiempo = pulseIn(pin_echo, HIGH);
  distancia = tiempo/58.3;
  Serial.print("Derecha tiempo: ");
  Serial.print(tiempo);
  Serial.print("   distancia: ");
  Serial.println(distancia);

  delay(1000);
}

void loop() {
  senDerecha();
  senIzquierda();
}
