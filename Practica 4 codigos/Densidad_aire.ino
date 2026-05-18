#include <Wire.h> //librería de Arduino para comunicación I2C.
#include <Adafruit_BMP085.h> //Libreria del sensor.

Adafruit_BMP085 bmp; //Se crea un objeto llamado "bmp" para usarla libreria más facil. 

void setup() {
  Serial.begin(9600); //inicia la comunicación serial a 9600 baudios.
  bmp.begin();  // inicializa el sensor. Internamente establece la conexión I2C.
}

void loop() {
  float P = bmp.readPressure(); //devuelve la presión en Pascales (Pa).
  float T = bmp.readTemperature() + 273.15; //devuelve la temperatura en °C, se le suman 273.15 para que sean °K.

  float R_esp = 287.68; //Constante del aire seco J/(Kg*°K), constante universal de los gases = 8.314J/K*mol, masa molar del aire = 0.029Kg/mol.
  //Ley de Gas ideal p=P/(R_cons)(T)   p=densidad del aire(Kg/m^3), P=presión absoluta(Pa), R_cosnt=Coantante del aire seco. 
  float densidad = P / (R_esp * T); //Calculo de la densiad. 

  //impresion de los valores. "println" genera un salto de linea.  
  Serial.print("Presión: "); Serial.print(P);  Serial.println(" Pa");
  Serial.print("Temperatura: "); Serial.print(T - 273.15); Serial.println(" °C");
  Serial.print("Densidad: ");   Serial.print(densidad, 4); Serial.println(" kg/m³");

  delay(1000);//pausa de 2 seg. 
}