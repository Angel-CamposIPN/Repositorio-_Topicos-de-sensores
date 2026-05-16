#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

int16_t gx, gy, gz;

float pitch = 0;
float roll = 0;

float dt = 0.01;
unsigned long tiempoAnterior;

void setup() {

  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();

  tiempoAnterior = millis();
}

void loop() {

  mpu.getRotation(&gx, &gy, &gz);

  unsigned long tiempoActual = millis();
  dt = (tiempoActual - tiempoAnterior) / 1000.0;
  tiempoAnterior = tiempoActual;

  float Gx = gx / 131.0;
  float Gy = gy / 131.0;

  pitch += Gx * dt;
  roll += Gy * dt;

  Serial.print("Pitch: ");
  Serial.print(pitch);
  Serial.print("  Roll: ");
  Serial.println(roll);
}