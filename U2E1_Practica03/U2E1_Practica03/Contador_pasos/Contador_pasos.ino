#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

int16_t ax, ay, az;
int pasos = 0;
bool enPaso = false;

// Umbrales en g
float umbralAlto = 1.4;
float umbralBajo = 1.1;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  Serial.println("Podometro listo");
}

void loop() {
  mpu.getAcceleration(&ax, &ay, &az);

  //Dividir entre 16384 → valores en g reales
  float Ax = (float)ax / 16384.0;
  float Ay = (float)ay / 16384.0;
  float Az = (float)az / 16384.0;

  float magnitud = sqrt(Ax*Ax + Ay*Ay + Az*Az);

  //Serial.print("G: ");
  //Serial.println(magnitud, 3); // ver el valor en reposo (~1.0)

  if (!enPaso && magnitud > umbralAlto) {
    enPaso = true;
  }

  if (enPaso && magnitud < umbralBajo) {
    enPaso = false;
    pasos++;
    Serial.print("Paso: ");
    Serial.println(pasos);
  }

  delay(10);
}