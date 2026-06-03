#include <MQUnifiedsensor.h>

#define BOARD   "Arduino UNO"
#define VOLTAGE  5.0
#define ADC_BIT  10
#define PIN_MQ   A0
#define RL       10      // kΩ — resistencia de carga del módulo
#define R0       13.5   // Ajustar tras calibración en aire limpio

MQUnifiedsensor MQ135(BOARD, VOLTAGE, ADC_BIT, PIN_MQ, "MQ-135");

void setup() {
  Serial.begin(9600);
  MQ135.init();
  MQ135.setRL(RL);
  MQ135.setR0(R0);

  // Encabezado CSV para Excel
  Serial.println("CO_ppm,Etanol_ppm,H2_ppm,Metano_ppm");
}

void loop() {
  MQ135.update(); // Lee el pin analógico

  MQ135.update();
  int rawADC = analogRead(PIN_MQ);
  float voltage = rawADC * (5.0 / 1023.0);
  float rs = ((5.0 * RL) / voltage) - RL;

  //Serial.print("ADC crudo: ");    Serial.println(rawADC);
  //Serial.print("Voltaje (V): ");  Serial.println(voltage, 3);
  //Serial.print("Rs (kΩ): ");      Serial.println(rs, 3);
  //Serial.print("Rs/R0: ");        Serial.println(rs / R0, 3);
  //delay(1000);
  
  // --- CO ---
  MQ135.setA(605.18); MQ135.setB(-3.937);
  float co = MQ135.readSensor();

  // --- Etanol ---
  MQ135.setA(77.255); MQ135.setB(-3.180);
  float etanol = MQ135.readSensor();

  // --- H2 ---
  MQ135.setA(19.74); MQ135.setB(-2.608);
  float h2 = MQ135.readSensor();

  // --- Metano ---
  MQ135.setA(458.79); MQ135.setB(-3.489);
  float metano = MQ135.readSensor();

  // Salida CSV
  Serial.print(co, 2);         Serial.print(",");
  Serial.print(etanol, 2);     Serial.print(",");
  Serial.print(h2, 2);         Serial.print(",");
  Serial.println(metano, 2);

  delay(1000);
}