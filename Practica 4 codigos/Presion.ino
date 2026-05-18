#include <Wire.h> //librería de Arduino para comunicación I2C.
#include <Adafruit_BMP085.h> //Libreria del sensor.

Adafruit_BMP085 bmp; //Se crea un objeto llamado "bmp" para usarla libreria más facil. 

void setup() {
  Serial.begin(9600); //inicia la comunicación serial a 9600 baudios.
  bmp.begin();  // inicializa el sensor. Internamente establece la conexión I2C.
}

void loop() {
  float P = bmp.readPressure(); //devuelve la presión en Pascales (Pa).

  //impresion de los valores. "println" genera un salto de linea.  
  Serial.print("Presión: "); Serial.print(P);  Serial.println(" Pa");

  delay(1000);//pausa de 2 seg. 
}