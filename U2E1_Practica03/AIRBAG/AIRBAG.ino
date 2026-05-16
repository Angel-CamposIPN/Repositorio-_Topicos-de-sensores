#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

int16_t ax, ay, az;
int led = 13;
float umbralImpacto = 2.5; // ahora en g

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("ERROR: MPU6050 no encontrado");
    while (true);
  }

  pinMode(led, OUTPUT);
  Serial.println("Sistema listo");
}

void loop() {
  mpu.getAcceleration(&ax, &ay, &az);

 //Convertir a float ANTES de operar
  float Ax = (float)ax / 16384.0;
  float Ay = (float)ay / 16384.0;
  float Az = (float)az / 16384.0;

  float fuerza = sqrt(Ax*Ax + Ay*Ay + Az*Az);

  Serial.print("FUERZA: ");
  Serial.print(fuerza, 3);
  Serial.println(" g");

  if (fuerza > umbralImpacto) {
    digitalWrite(led, HIGH);
    Serial.println(">>> AIRBAG ACTIVADO <<<");
  } else {
    digitalWrite(led, LOW);
    Serial.println("AIRBAG APAGADO");
  }

  delay(100);
}