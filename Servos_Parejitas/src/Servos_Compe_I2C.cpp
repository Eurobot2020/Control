/* 
 *  PROGRAMA CONTROLADOR DE LOS ACTUADORES
 *  USAREMOS EL DISPOSITIVO PCA9685.
 *  En el siguiente enlace se explica el 
 *  funcionamiento básico de este dispositivo:
 *  https://naylampmechatronics.com/blog/41_tutorial-modulo-controlador-de-servos-pca9685-con-arduino.html
 *  
 *  */

/* 
 *  Usaremos las siguientes bibliotecas para facilitar la programación.
 *  OJO. Está biblioteca está diseñada para usar A4 y A5.
 *  A4 -> SDA
 *  A5 -> SCL
 *
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

/*
 * Con el siguiente comando simplificamos la programación refiriendonos
 * a la dirección I2C del dispositivo como 'servos'.
 * OJO. ES MUY IMPORTANTE ESTA LÍNEA, SIN ELLA NO FUNCIONA EL PROGRAMA BIEN!
 */

Adafruit_PWMServoDriver servos = Adafruit_PWMServoDriver(0x40);


unsigned int H_level[16] = {0, 0, 0, 0, 0, 0, 0, 0, 275, 100, 170, 515, 415, 0, 0, 0};      
unsigned int L_level [16] = {0, 0, 0, 0, 0, 0, 0, 0, 150, 250, 290, 110, 0, 0, 0, 0};    

/*
    CALIBRACIÓN: 

        Bloq D:     H->170  L->300
        Bloq C:     H->275  L->150
        Brazo D:    H->220  L->100
        Brazo I:    H->515  L->110
        Bandera:    H->315  L->0
*/

/*
 * En las siguientes librerias definimos el movimiento, calibración y rutinas 
 * a llevar a cabo por los servos.
 */

#include "Movimiento_I2C.h"
#include "Rutinas_I2C.h"

  
void setup() 
{
  
  Config();
  Rutina_I();

}

void loop() 
{
  if(Serial.available()>0)
  {
    Instruccion_Codigo = Serial.read();
    
  switch(Instruccion_Codigo)
  {
   //Instrucciones que nos envian 
     case ('I'):
        Rutina_I();
        break;
      case ('A'):
        Rutina_A();
        break;
      case('C'):
        Rutina_C();
        break;
      case('S'):
        Rutina_S();
        break;
      case('L'):
        Rutina_L();
        break;
      case('B'):
        Rutina_B();
        break;
  }
  }
}
