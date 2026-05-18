#include <Wire.h> //Librería de Arduino para comunicación I2C.
#include <Adafruit_BMP085.h> //Librería del sensor.

Adafruit_BMP085 bmp; //Se crea un objeto llamado "bmp" para usar la librería más fácil. 

void setup() {
  Serial.begin(9600); //Inicia la comunicación serial a 9600 baudios.
  bmp.begin();  //Inicializa el sensor e internamente establece la conexión I2C.
}

void loop() {
  // Devuelve la altitud estimada en metros (m) basada en la presión atmosférica estándar.
  float altitud = bmp.readAltitude(); 

  // Impresión de los valores en el Monitor Serie.
  Serial.print("Altitud: "); 
  Serial.print(altitud);  
  Serial.println(" m");

  delay(1000); // Pausa de 1 segundo entre lecturas.
}
