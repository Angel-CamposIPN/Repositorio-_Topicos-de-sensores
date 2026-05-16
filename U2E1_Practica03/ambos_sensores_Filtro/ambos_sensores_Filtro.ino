#include <Wire.h>
#include <MPU6050.h>
#include <math.h>

MPU6050 mpu;

int16_t ax, ay, az;
int16_t gx, gy, gz;

float pitch = 0;
float roll = 0;

float alpha = 0.98;

unsigned long tiempoAnterior;

void setup() {

  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();

  tiempoAnterior = millis();
}

void loop() {

  unsigned long tiempoActual = millis();
  float dt = (tiempoActual - tiempoAnterior) / 1000.0;
  tiempoAnterior = tiempoActual;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float Ax = ax;
  float Ay = ay;
  float Az = az;

  float pitchAcc = atan2(-Ax, sqrt(Ay*Ay + Az*Az)) * 180/PI;
  float rollAcc = atan2(Ay, Az) * 180/PI;

  float Gx = gx / 131.0;
  float Gy = gy / 131.0;

  pitch = alpha * (pitch + Gx * dt) + (1 - alpha) * pitchAcc;
  roll  = alpha * (roll  + Gy * dt) + (1 - alpha) * rollAcc;

  Serial.print("Pitch: ");
  Serial.print(pitch);

  Serial.print("  Roll: ");
  Serial.println(roll);
}