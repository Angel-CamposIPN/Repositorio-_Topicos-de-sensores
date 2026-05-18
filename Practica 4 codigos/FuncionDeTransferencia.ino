#include <Wire.h>             //Libreria de Arduino para comunicación 12C.
#include <Adafruit_BMP085.h>  //Libreria del sensor.

Adafruit_BMP085 bmp; // Se crea un objeto llamado "bmp" para usar la líbrería más fácil.

void setup() {
    Serial.begin(9600); // Inicia la comunicación serial a 9600 baudios.
    bmp.begin();        // Inicializa el sensor. Internamente establece la conexión I2C.
}                       

void loop() {
    //BMP180
    float T_bmp = bmp.readTemperature(); // Devuelve la temperatura en ºC.

    //LM35 (A0).
    int lectura = analogRead(A0);             // Lectura cruda del ADC (0-1023).
    float voltaje = lectura * (5.0 / 1023.0); // Conversión a voltaje (asume 5V).
    float T_lm35 = voltaje * 100.0;           // El LM35 da 10mV/°C → T = V * 100.

    //Imprimir valores.            
    Serial.print(T_bmp);                   
    Serial.print(";");                     
    Serial.println(T_lm35);

    delay(2000); // Pausa de 2 seg.
}