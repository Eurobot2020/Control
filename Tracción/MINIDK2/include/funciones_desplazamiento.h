#ifndef _FUNCIONES_DESPLAZAMIENTO
#define _FUNCIONES_DESPLAZAMIENTO
#include "lpc17xx.h"
#include "funciones_pwm.h"
#include "variables.h"
#include <stdint.h>
#include "math.h"

#define FPCLK 				25e6																//Frecuencia de la LPC de 25MHz

void sentido_motores (int m_izquierdo, int m_derecho);                      //Cambia de sentido los motores
void configuracion_parametros_mecanicos (param_mecanicos *mecanica, cinematica *variable);			//Parametros mecanicos de controladora+motor
void velocidad_derecha (double rad_por_s,param_mecanicos *mecanica);			//Esta funci�n pone en la rueda derecha la velocidad que se le indica en rad/s
void velocidad_izquierda (double rad_por_s,param_mecanicos *mecanica);		//Esta funci�n pone en la rueda izquierda la velocidad que se le indica en rad/s
void motores (cinematica *variable, param_mecanicos *mecanica);
void apaga_motores (void);
void enciende_motores (void);
#endif
