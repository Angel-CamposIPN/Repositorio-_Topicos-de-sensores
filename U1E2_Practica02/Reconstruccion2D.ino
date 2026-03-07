#include <Wire.h>
#include <Adafruit_VL53L0X.h>

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
unsigned long startTime;
unsigned long currentTime;
bool escaneando = false; 
bool terminado = false;

const float velocidad = 50.0; // mm/s

void setup() {
  Serial.begin(115200);

  if (!lox.begin()) {
    Serial.println("Error al iniciar VL53L0X");
    while (1);
  }

  Serial.println(">>> SISTEMA EN ESPERA <<<");
  Serial.println("1. Presiona ENTER para INICIAR el escaneo.");
  Serial.println("2. Presiona ENTER de nuevo para FINALIZAR.");
}

void loop() {
  // Revisar si el usuario envió algo por el monitor serial
  if (Serial.available() > 0) {
    while(Serial.available() > 0) { Serial.read(); } // Limpiar buffer

    if (!escaneando && !terminado) {
      // INICIO DEL ESCANEO
      escaneando = true;
      Serial.println("--- INICIO DE DATOS ---");
      Serial.println("tiempo_ms,distancia_mm,x_mm,y_mm");
      startTime = millis();
    } 
    else if (escaneando) {
      // FIN DEL ESCANEO
      escaneando = false;
      terminado = true;
      Serial.println("--- ESCANEO FINALIZADO ---");
      Serial.println("Copia los datos de arriba y pégalos en MATLAB.");
    }
  }

  // Si estamos en modo escaneo, tomamos medidas
  if (escaneando) {
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);

    if (measure.RangeStatus != 4) {
      currentTime = millis() - startTime;
      float tiempo_seg = currentTime / 1000.0;
      float distancia = measure.RangeMilliMeter;

      float x = velocidad * tiempo_seg;  
      float y = distancia;               

      Serial.print(currentTime);
      Serial.print(",");
      Serial.print(distancia);
      Serial.print(",");
      Serial.print(x);
      Serial.print(",");
      Serial.println(y);
    }
    delay(50); // Muestreo cada 50ms
  }
}