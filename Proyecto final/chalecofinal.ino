// ============================================================
//  CHALECO MONITOR — Arduino Uno + MPU6050 + GPS Neo-6M 
//                    + BMP180
//  MODO CSV LOGGER — Datos por Serial (USB) listos para graficar
//
//  Formato CSV de salida:
//  timestamp_ms,tiempo_s,pasos,dist_acc_m,dist_gps_m,
//  lat,lon,satelites,ax_g,ay_g,az_g,magnitud,presion_pa,temp_c
//
//  Pines I2C  : SDA=A4, SCL=A5
//  GPS Neo-6M : RX=pin4, TX=pin5
// ============================================================

#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// ── GPS Neo-6M ────────────────────────────────────────────────
SoftwareSerial GPS_Serial(4, 5);  // RX=4, TX=5
TinyGPSPlus gps;

// ── MPU6050 ───────────────────────────────────────────────────
#define MPU 0x68

// ── BMP180 ────────────────────────────────────────────────────
#define BMP180_ADDR 0x77

// ── Parámetros de pasos ───────────────────────────────────────
#define UMBRAL_ALTO       1.2
#define UMBRAL_BAJO       0.8
#define INTERVALO_MIN_MS  250
#define LONGITUD_ZANCADA  0.75

// ── Variables de pasos ────────────────────────────────────────
long    pasos           = 0;
bool    esperandoPaso   = true;
unsigned long ultimoPasoMs = 0;
float   distanciaMetros = 0.0;

float   bufferAcc[5]   = {0};
int     idxAcc         = 0;

// ── Variables MPU (globales para exportar) ────────────────────
float   gAx = 0, gAy = 0, gAz = 0, gMag = 0;

// ── Variables BMP180 ──────────────────────────────────────────
float   gPresion_Pa = 0.0;
float   gTemp_C = 0.0;

// ── Variables de Detección de Caídas ──────────────────────────
#define UMBRAL_CAIDA      2.5      // g's para detectar caída
#define UMBRAL_IMPACTO    3.0      // g's para detectar impacto
#define VENTANA_CAIDA_MS  500      // Tiempo para confirmar caída
bool    enCaida         = false;
unsigned long tiempoInicioSospecha = 0;
bool    caida_detectada = false;
unsigned long tiempoUltimaCaida = 0;
float   aceleracionMaxima = 0.0;

// Coeficientes de calibración del BMP180
int16_t ac1, ac2, ac3;
uint16_t ac4, ac5, ac6;
int16_t b1, b2;
int16_t mb, mc, md;

// ── Variables GPS ─────────────────────────────────────────────
double  latAnterior    = 0.0;
double  lonAnterior    = 0.0;
bool    primerFijo     = false;
double  distanciaGPS_m = 0.0;

// ── Tiempo ────────────────────────────────────────────────────
unsigned long inicioMs    = 0;
unsigned long ultimoEnvio = 0;

// ─────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);   // Velocidad alta para no perder datos
  Wire.begin();
  delay(200);

  // Despertar MPU6050
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  delay(200);

  // Configurar rango acelerómetro ±2g
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);
  Wire.write(0x00);
  Wire.endTransmission(true);

  // Inicializar BMP180
  delay(200);
  bmp180_init();
  delay(200);

  // Iniciar GPS
  GPS_Serial.begin(9600);
  delay(100);

  // ── Cabecera CSV ─────────────────────────────────────────────
  // La app/monitor serial detecta esta línea para saber que empieza el stream
  Serial.println("##CHALECO_MONITOR_START##");
  Serial.println("timestamp_ms,tiempo_s,pasos,dist_acc_m,dist_gps_m,lat,lon,satelites,ax_g,ay_g,az_g,magnitud,presion_pa,temp_c,caida,aceleracion_max");

  inicioMs = millis();
}

// ─────────────────────────────────────────────────────────────
void loop() {
  unsigned long ahora = millis();
  unsigned long ciclo = ahora % 1000;

  // ── Primeros 900ms: escuchar GPS ─────────────────────────
  if (ciclo < 900) {
    while (GPS_Serial.available()) {
      gps.encode(GPS_Serial.read());
    }

    if (gps.location.isUpdated() && gps.location.isValid()) {
      double latActual = gps.location.lat();
      double lonActual = gps.location.lng();
      if (primerFijo) {
        double segmento = TinyGPSPlus::distanceBetween(
                            latAnterior, lonAnterior, latActual, lonActual);
        if (segmento < 50.0) distanciaGPS_m += segmento;
      }
      latAnterior = latActual;
      lonAnterior = lonActual;
      primerFijo  = true;
    }
  }

  // ── Leer MPU6050 siempre ─────────────────────────────────
  leerMPU6050();

  // ── Leer BMP180 cada 500ms (no precisa mayor frecuencia) ──
  static unsigned long ultimaLecturaBMP = 0;
  if (ahora - ultimaLecturaBMP >= 500) {
    ultimaLecturaBMP = ahora;
    leerBMP180();
  }

  // ── Cada 200ms: emitir fila CSV ──────────────────────────
  // (5 muestras/seg para buena resolución de gráficas)
  if (ahora - ultimoEnvio >= 200) {
    ultimoEnvio = ahora;
    unsigned long tiempoSeg = (ahora - inicioMs) / 1000;

    // Coordenadas GPS (0.000000 si sin señal)
    double lat = primerFijo ? latAnterior : 0.0;
    double lon = primerFijo ? lonAnterior : 0.0;
    int    sats = (int)gps.satellites.value();

    // ── Fila CSV por Serial (USB) ─────────────────────────
    Serial.print(ahora);          Serial.print(',');
    Serial.print(tiempoSeg);      Serial.print(',');
    Serial.print(pasos);          Serial.print(',');
    Serial.print(distanciaMetros, 2); Serial.print(',');
    Serial.print(distanciaGPS_m, 2);  Serial.print(',');
    Serial.print(lat, 6);         Serial.print(',');
    Serial.print(lon, 6);         Serial.print(',');
    Serial.print(sats);           Serial.print(',');
    Serial.print(gAx, 3);        Serial.print(',');
    Serial.print(gAy, 3);        Serial.print(',');
    Serial.print(gAz, 3);        Serial.print(',');
    Serial.print(gMag, 3);        Serial.print(',');
    Serial.print(gPresion_Pa, 1); Serial.print(',');
    Serial.print(gTemp_C, 2);     Serial.print(',');
    Serial.print(caida_detectada ? 1 : 0); Serial.print(',');
    Serial.println(aceleracionMaxima, 2);
  }
}

// ─────────────────────────────────────────────────────────────
//  LEER MPU6050 directo por I2C
// ─────────────────────────────────────────────────────────────
void leerMPU6050() {
  static unsigned long ultimaLectura = 0;
  if (millis() - ultimaLectura < 10) return;
  ultimaLectura = millis();

  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);

  int16_t ax = Wire.read() << 8 | Wire.read();
  int16_t ay = Wire.read() << 8 | Wire.read();
  int16_t az = Wire.read() << 8 | Wire.read();

  gAx = ax / 16384.0;
  gAy = ay / 16384.0;
  gAz = az / 16384.0;
  gMag = sqrt(gAx*gAx + gAy*gAy + gAz*gAz);

  // ── DETECCIÓN DE CAÍDAS ────────────────────────────────────
  unsigned long ahora = millis();
  
  // Actualizar aceleración máxima en la ventana actual
  if (gMag > aceleracionMaxima) {
    aceleracionMaxima = gMag;
  }

  // Máquina de estados para detección de caída
  if (!enCaida) {
    // Estado normal: esperar caída
    if (gMag > UMBRAL_CAIDA) {
      enCaida = true;
      tiempoInicioSospecha = ahora;
      caida_detectada = false;
      aceleracionMaxima = gMag;
    }
  } else {
    // Estado de sospecha: esperando impacto
    if (gMag > aceleracionMaxima) {
      aceleracionMaxima = gMag;
    }

    // Si la aceleración es muy alta = impacto confirmado
    if (gMag > UMBRAL_IMPACTO) {
      caida_detectada = true;
      tiempoUltimaCaida = ahora;
      
      // Emitir alerta inmediata por Serial
      Serial.println();
      Serial.println("***CAIDA_DETECTADA***");
      Serial.print("Aceleracion pico: ");
      Serial.print(aceleracionMaxima, 2);
      Serial.println(" g");
      Serial.println("***FIN_ALERTA***");
      Serial.println();
      
      enCaida = false;
      aceleracionMaxima = 0.0;
    }
    // Si pasó mucho tiempo sin impacto, volver a normal
    else if (ahora - tiempoInicioSospecha > VENTANA_CAIDA_MS) {
      enCaida = false;
      aceleracionMaxima = 0.0;
      caida_detectada = false;
    }
  }

  // Resetear bandera de caída después de registrarla
  static unsigned long ultimoResetoCaida = 0;
  if (caida_detectada && (ahora - ultimoResetoCaida > 2000)) {
    caida_detectada = false;
    ultimoResetoCaida = ahora;
    aceleracionMaxima = 0.0;
  }

  // Buffer suavizado para detección de pasos
  bufferAcc[idxAcc % 5] = gMag;
  idxAcc++;
  float promedio = 0;
  for (int i = 0; i < 5; i++) promedio += bufferAcc[i];
  promedio /= 5.0;

  if (esperandoPaso) {
    if (promedio > UMBRAL_ALTO && (ahora - ultimoPasoMs > INTERVALO_MIN_MS)) {
      pasos++;
      ultimoPasoMs    = ahora;
      distanciaMetros = pasos * LONGITUD_ZANCADA;
      esperandoPaso   = false;
    }
  } else {
    if (promedio < UMBRAL_BAJO) esperandoPaso = true;
  }
}

// ─────────────────────────────────────────────────────────────
//  INICIALIZAR BMP180
// ─────────────────────────────────────────────────────────────
void bmp180_init() {
  // Leer coeficientes de calibración (dirección 0xAA-0xBF)
  Wire.beginTransmission(BMP180_ADDR);
  Wire.write(0xAA);
  Wire.endTransmission();
  Wire.requestFrom(BMP180_ADDR, 22, true);

  // Forzamos el casteo correcto al leer los registros
  ac1 = (int16_t)((Wire.read() << 8) | Wire.read());
  ac2 = (int16_t)((Wire.read() << 8) | Wire.read());
  ac3 = (int16_t)((Wire.read() << 8) | Wire.read());
  ac4 = (uint16_t)((Wire.read() << 8) | Wire.read());
  ac5 = (uint16_t)((Wire.read() << 8) | Wire.read());
  ac6 = (uint16_t)((Wire.read() << 8) | Wire.read());
  b1  = (int16_t)((Wire.read() << 8) | Wire.read());
  b2  = (int16_t)((Wire.read() << 8) | Wire.read());
  mb  = (int16_t)((Wire.read() << 8) | Wire.read());
  mc  = (int16_t)((Wire.read() << 8) | Wire.read());
  md  = (int16_t)((Wire.read() << 8) | Wire.read());
}

void leerBMP180() {
  // 1. Leer temperatura sin calibrar (UT)
  Wire.beginTransmission(BMP180_ADDR);
  Wire.write(0xF4);
  Wire.write(0x2E);  // Comando lectura temperatura
  Wire.endTransmission();
  delay(5); // El BMP180 necesita mínimo 4.5ms

  Wire.beginTransmission(BMP180_ADDR);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP180_ADDR, 2, true);

  uint16_t ut = (Wire.read() << 8) | Wire.read();

  // 2. Leer presión sin calibrar (UP) con OSS=0 (modo estándar)
  Wire.beginTransmission(BMP180_ADDR);
  Wire.write(0xF4);
  Wire.write(0x34);  // Comando lectura presión, OSS=0
  Wire.endTransmission();
  delay(5);

  Wire.beginTransmission(BMP180_ADDR);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP180_ADDR, 3, true);

  uint32_t up = ((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 8) | Wire.read();
  up >>= 8;  // Desplazar 8 bits porque OSS=0

  // 3. Calibración de temperatura
  // Nota: Asegúrate de que x1 y x2 usen int32_t para evitar desbordamientos
  int32_t x1 = ((int32_t)ut - (int32_t)ac6) * (int32_t)ac5 >> 15;
  int32_t x2 = ((int32_t)mc << 11) / (x1 + md);
  int32_t b5 = x1 + x2;
  
  // Cálculo final de temperatura en grados Celsius
  int32_t t_entera = (b5 + 8) >> 4; // Da la temperatura multiplicada por 10
  gTemp_C = t_entera / 10.0;        // Almacena el float real (ej: 25.3)

  // 4. Calibración de presión
  int32_t b6 = b5 - 4000;
  x1 = (b2 * ((b6 * b6) >> 12)) >> 11;
  x2 = (ac2 * b6) >> 11;
  int32_t x3 = x1 + x2;
  
  // Ojo con los paréntesis aquí para respetar la hoja de datos
  int32_t b3 = ((((int32_t)ac1 * 4 + x3) << 0) + 2) >> 2; // OSS = 0
  
  x1 = (ac3 * b6) >> 13;
  x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  uint32_t b4 = (ac4 * (uint32_t)(x3 + 32768)) >> 15;
  uint32_t b7 = ((uint32_t)up - b3) * (50000 >> 0);  // OSS=0
  
  int32_t p;
  if (b7 < 0x80000000) {
    p = (b7 << 1) / b4;
  } else {
    p = (b7 / b4) << 1;
  }
  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  gPresion_Pa = p + ((x1 + x2 + 3791) >> 4);
}
