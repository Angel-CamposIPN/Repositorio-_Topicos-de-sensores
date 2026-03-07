#include "Adafruit_VL53L0X.h"

// Creamos el objeto del sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);

  // Esperar a que el monitor serie se abra
  while (!Serial) { delay(1); }

  Serial.println("Iniciando sensor VL53L0X...");
  
  // Iniciar el sensor
  if (!lox.begin()) {
    Serial.println(F("¡Error al encontrar el sensor! Revisa el cableado."));
    while (1);
  }
  
  Serial.println(F("Sensor listo. Midiendo distancia... \n"));
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;
    
  Serial.print("Midiendo... ");
  lox.rangingTest(&measure, false); // "false" para no mostrar debug por serie

  // El estado 4 significa que la medición es válida
  if (measure.RangeStatus != 4) {  
    Serial.print("Distancia (mm): ");
    Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println(" Fuera de rango ");
  }
    
  delay(100);
}