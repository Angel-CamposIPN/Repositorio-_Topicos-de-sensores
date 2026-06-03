#include <MQUnifiedsensor.h>

#define BOARD   "Arduino UNO"
#define VOLTAGE  5.0
#define ADC_BIT  10
#define PIN_MQ   A0
#define RL       10
#define R0       13.5

#define PIN_BUZZER 8   // Buzzer en pin digital 8

MQUnifiedsensor MQ135(BOARD, VOLTAGE, ADC_BIT, PIN_MQ, "MQ-135");

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  MQ135.init();
  MQ135.setRL(RL);
  MQ135.setR0(R0);
  MQ135.setA(77.255);
  MQ135.setB(-3.180);

  Serial.println("=== ALCOHOLIMETRO MQ-135 ===");
  Serial.println("Timestamp_ms,Etanol_ppm,BAC_g/L,Nivel");
}

float ppmToBAC(float ppm) {
  return ppm * 0.00021;
}

String nivelBAC(float bac) {
  if (bac < 0.1)  return "SOBRIO";
  if (bac < 0.5)  return "MINIMO";
  if (bac < 0.8)  return "PRECAUCION";
  if (bac < 1.5)  return "EBRIO";
  return          "PELIGROSO";
}

// Patrones de buzzer según nivel
void alertaBuzzer(String nivel) {
  if (nivel == "SOBRIO" || nivel == "MINIMO") {
    digitalWrite(PIN_BUZZER, LOW);  // Sin sonido

  } else if (nivel == "PRECAUCION") {
    // 2 beeps cortos
    tone(PIN_BUZZER, 1000, 150);  delay(300);
    tone(PIN_BUZZER, 1000, 150);  delay(300);

  } else if (nivel == "EBRIO") {
    // 4 beeps rápidos
    for (int i = 0; i < 4; i++) {
      tone(PIN_BUZZER, 1500, 100); delay(200);
    }

  } else if (nivel == "PELIGROSO") {
    // Alarma continua ascendente
    for (int freq = 800; freq <= 2000; freq += 100) {
      tone(PIN_BUZZER, freq, 60);  delay(70);
    }
  }
}

void loop() {
  MQ135.update();
  float ppm  = MQ135.readSensor();
  float bac  = ppmToBAC(ppm);
  String nivel = nivelBAC(bac);

  // CSV para Excel
  Serial.print(millis());  Serial.print(",");
  Serial.print(ppm, 2);    Serial.print(",");
  Serial.print(bac, 4);    Serial.print(",");
  Serial.println(nivel);

  alertaBuzzer(nivel);

  delay(500);
}