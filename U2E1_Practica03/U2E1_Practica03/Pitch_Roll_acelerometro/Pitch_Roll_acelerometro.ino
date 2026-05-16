#include <Wire.h>
#include <MPU6050.h>
#include <math.h>

MPU6050 mpu;

int16_t ax, ay, az;
float pitch, roll;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
}

void loop() {

  mpu.getAcceleration(&ax, &ay, &az);

  float Ax = ax;
  float Ay = ay;
  float Az = az;

  roll = atan2(Ay, Az) * 180 / PI;
  pitch = atan2(-Ax, sqrt(Ay * Ay + Az * Az)) * 180 / PI;

  Serial.print("Pitch: ");
  Serial.print(pitch);
  Serial.print("  Roll: ");
  Serial.println(roll);

  delay(200);
}