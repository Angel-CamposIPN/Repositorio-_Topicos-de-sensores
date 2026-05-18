#include <Wire.h> //librería de Arduino para comunicación I2C.
#include <Adafruit_BMP085.h> //Libreria del sensor.
#include <EEPROM.h> //Librería para usar la memoria EEPROM del Arduino.

Adafruit_BMP085 bmp; //Se crea un objeto llamado "bmp" para usarla libreria más facil. 

// Direcciones en EEPROM donde se guardan los datos.
// Un número float ocupa 4 bytes, así que:
// Tmax → dirección 0-3
// Tmin → dirección 4-7
#define ADDR_TMAX 0
#define ADDR_TMIN 4

int Tb = 10;  //Temperatura base o umbral °C.
float Tmax, Tmin; // Se declaran sin valor inicial, lo asignará la EEPROM en setup().
String etapa;

void setup() {
  Serial.begin(9600); //inicia la comunicación serial a 9600 baudios.
  bmp.begin();  // inicializa el sensor. Internamente establece la conexión I2C.

  // Recuperar Tmax y Tmin guardados en EEPROM de la sesión anterior.
  EEPROM.get(ADDR_TMAX, Tmax);
  EEPROM.get(ADDR_TMIN, Tmin);

  // Validar que los datos recuperados no sean basura.
  // Si la EEPROM nunca se escribió, contiene 0xFF que se lee como NaN o valor absurdo.
  if (isnan(Tmax) || Tmax > 100 || Tmax < -55) Tmax = -999;
  if (isnan(Tmin) || Tmin > 100 || Tmin < -55) Tmin = 999;

  Serial.println("=== Valores recuperados de EEPROM ===");
  Serial.print("Tmax: "); Serial.println(Tmax);
  Serial.print("Tmin: "); Serial.println(Tmin);
  Serial.println("=====================================");
}

void loop() {

  float T = bmp.readTemperature(); //devuelve la temperatura en °C.

  // Actualizar máximo y mínimo, y guardar en EEPROM SOLO si cambian.
  if (T > Tmax) {
    Tmax = T;
    EEPROM.put(ADDR_TMAX, Tmax); // Guardar nuevo máximo en EEPROM.
  }
  if (T < Tmin) {
    Tmin = T;
    EEPROM.put(ADDR_TMIN, Tmin); // Guardar nuevo mínimo en EEPROM.
  }

  float UnidadesCalor = (Tmax + Tmin) / 2.0 - Tb; //Formula de las unidades de calor.

  //Etapas fenologicas
  if (UnidadesCalor < 12.89) {
    etapa = "Sin etapa";
  } else if (UnidadesCalor < 64.41) {
    etapa = "Emergencia";
  } else if (UnidadesCalor < 91.33) {
    etapa = "Formacion de guias";
  } else if (UnidadesCalor < 99.31) {
    etapa = "Floracion";
  } else if (UnidadesCalor < 112.77) {
    etapa = "Formacion de vaina";
  } else if (UnidadesCalor < 153.24) {
    etapa = "Llenado de vainas";
  } else {
    etapa = "Maduracion";
  }

  //impresion de los valores. "println" genera un salto de linea.  
  Serial.print("Temperatura: "); Serial.print(T); Serial.println(" °C");
  Serial.print("Tmax: "); Serial.print(Tmax); Serial.print(" | Tmin: "); Serial.println(Tmin);
  Serial.print("Unidades de calor: "); Serial.println(UnidadesCalor);
  Serial.print("Etapa fenologica: "); Serial.println(etapa);
  Serial.println("---");

  delay(10000);//pausa de 10 seg. 
}