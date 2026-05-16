#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

int16_t ax, ay, az;

// ─── Configuración del experimento ───────────────────
const float MASA_KG       = 0.5;   // masa del objeto en kg (ajusta)
const float UMBRAL_INICIO = 1.15;  // g — detectar inicio del empuje
const float UMBRAL_FIN    = 1.02;  // g — detectar fin del empuje
const int   INTENTOS_META = 10;    // repeticiones del experimento

// ─── Variables ────────────────────────────────────────
int     intento       = 0;
bool    empujando     = false;
float   fuerzaMax     = 0;
float   acumG         = 0;
int     muestras      = 0;

// Historial de resultados
float fuerzasRegistradas[10];

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();

  Serial.println("=== Experimento: Fuerza de empuje ===");
  Serial.print("Masa del objeto: ");
  Serial.print(MASA_KG);
  Serial.println(" kg");
  Serial.println("Empuje el objeto entre 3 y 10 cm.");
  Serial.println("Intento | F_max(N) | F_prom(N) | a(m/s²)");
  Serial.println("----------------------------------------");

void loop() {

  if (intento >= INTENTOS_META) {
    mostrarResumen();
    while (true); // detener al completar
  }

  mpu.getAcceleration(&ax, &ay, &az);

  float Ax = (float)ax / 16384.0;
  float Ay = (float)ay / 16384.0;
  float Az = (float)az / 16384.0;

  // Magnitud total y conversión a m/s²
  float G_total  = sqrt(Ax*Ax + Ay*Ay + Az*Az);
  float AceleracionInicial = (G_total - 1.0) * 9.8; // restar 1g de gravedad
  if (AceleracionInicial < 0) AceleracionInicial = 0;

  // ── Detectar inicio del empuje ────────────────────
  if (!empujando && G_total > UMBRAL_INICIO) {
    empujando = true;
    fuerzaMax = 0;
    acumG     = 0;
    muestras  = 0;
  }

  // ── Durante el empuje ─────────────────────────────
  if (empujando) {
    float fuerza = MASA_KG * AceleracionInicial;
    if (fuerza > fuerzaMax) fuerzaMax = fuerza;
    acumG += G_total;
    muestras++;
  }

  // ── Detectar fin del empuje ───────────────────────
  if (empujando && G_total < UMBRAL_FIN) {
    empujando = false;
    intento++;

    float G_promedio     = acumG / muestras;
    float acelpromedio = (G_promedio - 1.0) * 9.8;
    if (acelpromedio < 0) acelpromedio = 0;
    float fuerzaProm = MASA_KG * acelpromedio;

    fuerzasRegistradas[intento - 1] = fuerzaMax;

    Serial.print("  ");
    Serial.print(intento);
    Serial.print("     | ");
    Serial.print(fuerzaMax, 3);
    Serial.print(" N   | ");
    Serial.print(fuerzaProm, 3);
    Serial.print(" N    | ");
    Serial.print(accel_prom_ms2, 3);
    Serial.println(" m/s²");
  }

  delay(10);
}

void mostrarResumen() {
  Serial.println("\n=== RESUMEN DE 10 INTENTOS ===");

  float suma  = 0;
  float fMax  = 0;
  float fMin  = fuerzasRegistradas[0];

  for (int i = 0; i < INTENTOS_META; i++) {
    suma += fuerzasRegistradas[i];
    if (fuerzasRegistradas[i] > fMax) fMax = fuerzasRegistradas[i];
    if (fuerzasRegistradas[i] < fMin) fMin = fuerzasRegistradas[i];
  }

  float promedio = suma / INTENTOS_META;

  // Desviación estándar
  float varianza = 0;
  for (int i = 0; i < INTENTOS_META; i++) {
    float diff = fuerzasRegistradas[i] - promedio;
    varianza += diff * diff;
  }
  float desviacion = sqrt(varianza / INTENTOS_META);

  Serial.print("Fuerza maxima registrada : "); Serial.print(fMax, 3);      Serial.println(" N");
  Serial.print("Fuerza minima registrada : "); Serial.print(fMin, 3);      Serial.println(" N");
  Serial.print("Fuerza promedio          : "); Serial.print(promedio, 3);  Serial.println(" N");
  Serial.print("Desviacion estandar      : "); Serial.print(desviacion, 3);Serial.println(" N");
  Serial.println("\nExperimento completado.");
}