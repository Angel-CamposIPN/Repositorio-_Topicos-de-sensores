#include <SoftwareSerial.h>
#include <TinyGPS++.h>

static const int RXPin = 4;
static const int TXPin = 3;
static const uint32_t GPSBaud = 9600;

const int buzzerPin = 8;

SoftwareSerial gpsSerial(RXPin, TXPin);
TinyGPSPlus gps;

// Punto central
double centroLat = 22.783575;
double centroLng = -102.614936;

double radio = 50.0; // metros

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {

  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isValid()) {

    double distancia = TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      centroLat,
      centroLng
    );

    Serial.print("Distancia: ");
    Serial.println(distancia);

    if (distancia > radio) {
      digitalWrite(buzzerPin, HIGH); // Se salió
    } else {
      digitalWrite(buzzerPin, LOW);  // Dentro
    }
  }
}
