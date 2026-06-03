#include <MQUnifiedsensor.h>

// Definiciones
#define PLACA        "Arduino UNO"
#define VOLTAJE      5.0
#define RESOLUCION   10     // 10 bits para Arduino UNO
#define PIN_MQ135    A0
#define TIPO         "MQ-135"
#define RL           10     // Resistencia de carga en kΩ

MQUnifiedsensor mq135(PLACA, VOLTAJE, RESOLUCION, PIN_MQ135, TIPO);

void setup() {
  Serial.begin(9600);

  // Configurar regresión (curva del datasheet)
  mq135.setRegressionMethod(1); // Método exponencial

  mq135.init();

  // ── Calibración ──────────────────────────────────────
  Serial.println("Calibrando, espera 20 segundos...");
  float calcRo = 0;
  for (int i = 0; i < 10; i++) {
    mq135.update();
    calcRo += mq135.calibrate(3.6); // 3.6 = Rs/Ro en aire limpio (datasheet)
    delay(2000);
  }
  mq135.setR0(calcRo / 10); // Promedio de 10 lecturas

  Serial.print("Ro calibrado = ");
  Serial.println(calcRo / 10);
  Serial.println("Listo!\n");
  Serial.println("CO2_ppm,CO_ppm,NH3_ppm,Alcohol_ppm");
}

void loop() {
  mq135.update(); // Leer sensor

  //── CO2 ──────────────────────────────────────────────
  mq135.setA(110.47); mq135.setB(-2.862);
  float co2 = mq135.readSensor() + 400; // +400 ppm base atmosférica
  
  //── CO ───────────────────────────────────────────────
  mq135.setA(605.18); mq135.setB(-3.937);
  float co = mq135.readSensor();

  //── NH3 (Amoniaco) ───────────────────────────────────
  mq135.setA(102.2); mq135.setB(-2.473);
  float nh3 = mq135.readSensor();

  //── Alcohol ──────────────────────────────────────────
  mq135.setA(77.255); mq135.setB(-3.18);
  float alcohol = mq135.readSensor();

  // ── Mostrar resultados ────────────────────────────────

// Timestamp en milisegundos desde el inicio
  Serial.print(co2);
  Serial.print(",");
  Serial.print(co);
  Serial.print(",");
  Serial.print(nh3);
  Serial.print(",");
  Serial.println(alcohol);  // println al final para salto de línea
  delay(2000);
}