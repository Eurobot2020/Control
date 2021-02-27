#include "lpc17xx.h"
#include "variables.h"
#include "funciones_pwm.h"
#include <stdint.h>
#include "math.h"


#define FPCLK 				25e6																//Frecuencia de la LPC de 25MHz

//NOTA IMPORTANTE: he definido el struct en un fichero '.h' aparte para que pueda acceder a los campos de esta estructura desde cualquier 
//fichero '.c' que haya creado. Adem�s para asegurarme que se cambian los valores de los campos del struct he usado punteros donde envio la
//direccion de memoria del struct

void sentido_motores (int m_izquierdo, int m_derecho)
{
	LPC_GPIO3->FIOCLR|=(3<<25); //LIMPIAMOS LOS PUERTOS

	if(m_izquierdo)
	{
		LPC_GPIO3->FIOPIN3&=~(0x01<<2);			//AVANZA EL IZQUIERDO
	}
	else 
	{
		LPC_GPIO3->FIOPIN3|=(0x01<<2);			//RETROCEDE EL IZQUIERDO
	}

	if(m_derecho)
	{
		LPC_GPIO3->FIOPIN3&=~(0x01<<1);			//AVANZA EL DERECHO
	}
	else
	{
		LPC_GPIO3->FIOPIN3|=(0x01<<1);			//RETROCEDE EL DERECHO
	}
	
}

void velocidad_derecha (double rad_por_s,param_mecanicos *mecanica)		//Esta funci�n pone en la rueda derecha la velocidad que se le indica en rad/s
{
	PWM2_SetDuty((float)rad_por_s*800/mecanica->vel_max);
}

void velocidad_izquierda (double rad_por_s,param_mecanicos *mecanica)	//Esta funci�n pone en la rueda izquierda la velocidad que se le indica en rad/s
{
	PWM3_SetDuty((float)rad_por_s*800/mecanica->vel_max);
}


void apaga_motores (void)
{
	LPC_GPIO2->FIOCLR1	|=	(3<<3);	
	LPC_GPIO2->FIOCLR1	|=	(1<<2);
}

void enciende_motores (void)
{
	LPC_GPIO2->FIOSET1 |=	(3<<3);
	LPC_GPIO2->FIOSET1 |= (1<<2);
}

void motores (cinematica *variable, param_mecanicos *mecanica)
{
	//Ponemos las velocidades en las ruedas
	
	velocidad_derecha(variable->velocidad_final, mecanica);
	velocidad_izquierda(variable->velocidad_final, mecanica);
	enciende_motores();
	
}
