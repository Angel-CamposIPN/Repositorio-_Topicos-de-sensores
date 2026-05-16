#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

int16_t ax, ay, az;

void setup() {

  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
}

void loop() {

  mpu.getAcceleration(&ax, &ay, &az);

  Serial.print(ax);
  Serial.print(",");
  Serial.print(ay);
  Serial.print(",");
  Serial.println(az);

  delay(10);
}