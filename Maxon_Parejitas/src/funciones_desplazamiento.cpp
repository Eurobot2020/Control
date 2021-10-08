#include <Arduino.h>
#include "variables.h"
#include "funciones_pwm.h"
#include <stdint.h>
#include "math.h"
// #include "PinChangeInterrupt.h"

// Pines de controladoras Maxon Derecha OCR0B
#define D_STOP 2
#define D_DIR 3
#define D_EN  4
#define D_PWM 5
// Pines de controladoras Maxon Izquierda OCR0A
#define I_PWM 6
#define I_EN  7
#define I_DIR 8
#define I_STOP 9

extern bool sentido_dcha;
extern bool sentido_izq;
//NOTA IMPORTANTE: he definido el struct en un fichero '.h' aparte para que pueda acceder a los campos de esta estructura desde cualquier 
//fichero '.c' que haya creado. Adem�s para asegurarme que se cambian los valores de los campos del struct he usado punteros donde envio la
//direccion de memoria del struct

void sentido_motores (int m_izquierdo, int m_derecho)
{
	digitalWrite(I_DIR,LOW);					//LIMPIAMOS LOS PUERTOS
	digitalWrite(D_DIR,LOW);

	if(m_izquierdo)
	{
		digitalWrite(I_DIR,LOW);			//AVANZA EL IZQUIERDO
		sentido_izq = 1;					//SinPosa = HIGH //Parejitas = LOW
	}
	else 
	{
		digitalWrite(I_DIR,HIGH);			//RETROCEDE EL IZQUIERDO
		sentido_izq = 0;					//SinPosa = LOW //Parejitas = HIGH
	}

	if(m_derecho)
	{
		digitalWrite(D_DIR,HIGH);			//AVANZA EL DERECHO
		sentido_dcha = 1;					//SinPosa = LOW //Parejitas = HIGH
	}
	else
	{
		digitalWrite(D_DIR,LOW);			//RETROCEDE EL DERECHO
		sentido_dcha = 0;					//SinPosa = HIGH //Parejitas = LOW
	}
	
}

void velocidad_derecha (double rad_por_s,param_mecanicos *mecanica)		//Esta funci�n pone en el motor derecho la velocidad que se le indica en rad/s
{
	if (rad_por_s>mecanica->vel_max) //Analizo que los módulos estén dentro de los límites mecánicos de los motores Maxon
	{
		PWM2_SetDuty((float)1); 	//Máximo ciclo de trabajo go brrr
	}
	else
		PWM2_SetDuty((float)rad_por_s/mecanica->vel_max);
}

void velocidad_izquierda (double rad_por_s,param_mecanicos *mecanica)	//Esta funci�n pone en el motor izquierdo la velocidad que se le indica en rad/s
{
	if (rad_por_s>mecanica->vel_max) //Analizo que los módulos estén dentro de los límites mecánicos de los motores Maxon
	{
		PWM3_SetDuty((float)1);	//Máximo ciclo de trabajo go brrr
	}
	else
		PWM3_SetDuty((float)rad_por_s/mecanica->vel_max);
}


void apaga_motores (void)
{
	PORTD &= ~(B10010000);
	// digitalWrite(I_EN,LOW);
	// digitalWrite(D_EN,LOW);
}

void enciende_motores (void)
{
	PORTD |= B10010000;
	// digitalWrite(I_EN,HIGH);
	// digitalWrite(D_EN,HIGH);
}

void motores (cinematica *variable, param_mecanicos *mecanica)
{
	//Ponemos las velocidades en las ruedas
	
	velocidad_derecha(variable->velocidad_final, mecanica);
	velocidad_izquierda(variable->velocidad_final, mecanica);
	enciende_motores();
	
}
