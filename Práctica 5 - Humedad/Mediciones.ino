#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP085.h>

Adafruit_AHTX0 aht;
Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);
  delay(1000);
  Wire.begin();
  aht.begin();
  Wire.end();
  delay(200);
  Wire.begin();
  delay(200);
  bmp.begin();
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  float tempBMP = bmp.readTemperature();

  Serial.print(tempBMP);
  Serial.print(",");
  Serial.print(temp.temperature);
  Serial.print(",");
  Serial.println(humidity.relative_humidity);

  delay(2000);
}