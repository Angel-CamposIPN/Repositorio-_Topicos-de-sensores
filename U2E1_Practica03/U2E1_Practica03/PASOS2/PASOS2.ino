#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// ─── Parámetros ajustables ────────────────────────────
const float UMBRAL_ALTO  = 1.4;  // g — sube: inicio de paso
const float UMBRAL_BAJO  = 1.1;  // g — baja: confirmar paso
const int   TIEMPO_MIN   = 250;  // ms mínimos entre pasos (evita doble conteo)
const float META_PASOS   = 10000; // pasos diarios meta (opcional)

// ─── Variables de estado ──────────────────────────────
int   pasos          = 0;
bool  enPaso         = false;
unsigned long ultimoPaso = 0;

// ─── Conversión a distancia y calorías ────────────────
const float LONGITUD_PASO_M = 0.75; // metros por paso (promedio adulto)
const float CALORIAS_POR_PASO = 0.04; // kcal por paso aproximado

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("ERROR: MPU6050 no encontrado");
    while (true);
  }

  // Escala ±2g para máxima sensibilidad al caminar
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  Serial.println("=== Podometro iniciado ===");
  Serial.println("Pasos | Dist(m) | Cal(kcal)");
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Convertir a g
  float Ax = (float)ax / 16384.0;
  float Ay = (float)ay / 16384.0;
  float Az = (float)az / 16384.0;

  // Magnitud total
  float G = sqrt(Ax*Ax + Ay*Ay + Az*Az);

  unsigned long ahora = millis();

  // ── Máquina de estados ────────────────────────────────
  if (!enPaso && G > UMBRAL_ALTO) {
    // La señal subió del umbral → posible inicio de paso
    enPaso = true;
  }

  if (enPaso && G < UMBRAL_BAJO) {
    // La señal bajó → paso confirmado
    enPaso = false;

    // Validar tiempo mínimo entre pasos (anti-rebote)
    if ((ahora - ultimoPaso) > TIEMPO_MIN) {
      pasos++;
      ultimoPaso = ahora;

      float distancia = pasos * LONGITUD_PASO_M;
      float calorias  = pasos * CALORIAS_POR_PASO;

      Serial.print(pasos);
      Serial.print("     | ");
      Serial.print(distancia, 1);
      Serial.print(" m    | ");
      Serial.print(calorias, 1);
      Serial.println(" kcal");
    }
  }

  delay(10); // muestreo a 100 Hz
}