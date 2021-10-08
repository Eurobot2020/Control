// En el bucle de ejecución estamos constantemente a la espera de un mensaje.
// Según el mensaje aplicaremos una u otras rutinas.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

void Rutina_I(void) 
{
    // Llevamos todo a su posición inicial
    L_Servo(Bloq_D);
    L_Servo(Bloq_C);
    L_Servo(Brazo_D);
    L_Servo(Brazo_I);
    L_Servo(Bandera);
    
    //Mensaje Recibido
    // Serial.print("Rutina de inicio \n");
    Serial.print("L\n");
    
}


void Rutina_A(void)
{
    //Abrimos los brazos y bloqueadores
    L_Servo(Bloq_D);
    L_Servo(Bloq_C);
    H_Servo(Brazo_D);
    M_Servo(Brazo_I);

    //Mensaje Recibido
    // Serial.print("Apertura de los brazos \n");
    Serial.print("L\n");
}


void Rutina_B(void)
{
    /*
    * Para subir la bandera activamos el servo, 
    * ejecutamos un delay() y seguidamente desactivamos
    * de nuevo el servo.
    */

    H_Servo(Bandera);
    delay(1500);         // Hay que calcular el tiempo que tarda
    L_Servo(Bandera);

    //Mensaje Recibido
    // Serial.print("Activamos el servo de la bandera \n");
    Serial.print("L\n");
}


void Rutina_C(void)
{
    //Cerramos brazos y bloqueadores
    H_Servo(Bloq_D);
    H_Servo(Bloq_C);
    H_Servo(Brazo_D);
    M_Servo(Brazo_I);    


    //Mensaje Recibido
    // Serial.print("Cerramos  \n");
    Serial.print("L\n");

}

void Rutina_S(void)
{
    //Lleva el brazo a la posición de golpeo
    L_Servo(Bloq_D);
    L_Servo(Bloq_C);
    L_Servo(Brazo_D);
    H_Servo(Brazo_I);

    //Mensaje Recibido
    // Serial.print("Abrimos el brazo izquierdo  \n");
    Serial.print("L\n");

}

void Rutina_L(void)
{
    //Llevamos el brazo izquierdo a la posición neutral
    M_Servo(Brazo_I);

    //Mensaje Recibido
    // Serial.print("Abrimos el brazo izquierdo  \n");
    Serial.print("L\n");

}

