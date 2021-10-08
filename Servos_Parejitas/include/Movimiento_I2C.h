/*
 * Simplemente renombramos el movimiento para que sea más comprensible
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVO_FREQ 50   // Definimos la frecuencia de trabajo de los servos.
#define M_level    415  // Definimos el valor correspondiente a la posición intermedia del brazo izquierdo.

/*
 * Para aplicaciones en las que necesitemos un control absoluto 
 * sobre todas las posibles posiciones del servo sería óptimo 
 * hacer un mapeo en el que cambiemos cuentas/ticks en grados.
 * En este aplicación calibramos los servos y definimos un 'High
 * level' y un 'Low level'. 
 * OJO. El brazo izquierdo del robot tiene una tercera posición.
 * Almacenaremos los 'ticks' correspondientes a cada nivel y servo
 * en vectores.
 * El orden de almacenamiento en el vector es el siguiente:
 * 
 * H/L [5] = {Bloq_D, Bloq_C, Brazo_D, Brazo_I, Bandera}
 * 
 * Por lo tanto, definimos con estos indices a los servos.
 * 
 * El rango teórico de funcionamiento es de 172(0º) a 565(180º).
 * 
*/

#define Bloq_D    10
#define Bloq_C    8
#define Brazo_D   9
#define Brazo_I   11
#define Bandera   12


    /*  BRAZO IZQUIERDO   */

/*
 * OJO. Mucho cuidado al calibrar el servo 
 * del brazo izquierdo. Hay valores para los 
 * que aparentemente funciona correctamente, 
 * alcanza su posición y no vibra, pero se quema 
 * por dentro. Se nota claramente por el aumento 
 * de la temperatura en la superficie del servo.
*/

    /*  BANDERA */

/*
 * El servo encargado de subir la bandera
 * está trucado, por ello, para un señal 
 * concreta se queda parado, y para el resto 
 * de señales posibles se mueve. La cercanía 
 * entre H_level[Bandera] y L_level[Bnadera]
 * se debe a que señales muy alejadas del
 * estado 'parado' hacen que se mueva excesivamente rápido.
*/

// Variable de lectura del Monitor Serie
char Instruccion_Codigo = 'W';  //Establecemos una instrucción no válida para evitar que los servos se muevan innecesariamente.

void Config(void)
{
    /* CONFIGURACIÓN DE LOS SERVOS */

    servos.begin();
    servos.setPWMFreq(SERVO_FREQ);

    /* INICIACIÓN DEL MONITOR SERIE */

    Serial.begin(115200);
    
}

void H_Servo(int SERVO)
{
    servos.setPWM(SERVO, 0, H_level[SERVO]);
}

void L_Servo(int SERVO)
{
    servos.setPWM(SERVO, 0, L_level[SERVO]);
}

void M_Servo(int SERVO)
{
    /* FUNCIÓN ESPECIAL ESPECIFICA PARA LA POSICIÓN INTERMEDIA DEL BRAZO IZQUIERDO */
    servos.setPWM(SERVO, 0, M_level);
}
