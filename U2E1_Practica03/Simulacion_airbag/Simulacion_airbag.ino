#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

int16_t ax, ay, az;

int led = 13;

float umbralImpacto = 25000;

void setup() {

  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();

  pinMode(led, OUTPUT);
}

void loop() {

  mpu.getAcceleration(&ax, &ay, &az);
  float Ax=float(ax);
  float Ay=float(ay);
  float Az=float(az);

  float fuerza = sqrt(Ax*Ax + Ay*Ay + Az*Az);
  Serial.println("PRUEBA DE AIRBAG");
  Serial.print("FUERZA:"); Serial.println(fuerza);

  if(fuerza > umbralImpacto){

    digitalWrite(led, HIGH);
    Serial.println("AIRBAG ACTIVADO");

  }else{

    digitalWrite(led, LOW);
    Serial.println("AIRBAG APAGADO");
  }

}