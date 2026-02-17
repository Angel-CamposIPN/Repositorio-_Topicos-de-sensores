#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Pines para el GPS
static const int RXPin = 4; // GPS TX
static const int TXPin = 3; // GPS RX
static const uint32_t GPSBaud = 9600;

// Objetos
SoftwareSerial gpsSerial(RXPin, TXPin);
TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);

  // Encabezado de la tabla
  Serial.println("latitud,longitud,altitud_m");
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());

    if (gps.location.isUpdated()) {
      Serial.print(gps.location.lat(), 6);
      Serial.print(",");
      Serial.print(gps.location.lng(), 6);
      Serial.print(",");
      
      if (gps.altitude.isValid()) {
        Serial.println(gps.altitude.meters());
      } else {
        Serial.println("0");
      }

      //delay(1000); // 1 punto por segundo
    }
  }
}
