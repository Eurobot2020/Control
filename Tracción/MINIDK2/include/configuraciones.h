#ifndef _CONFIGURACIONES
#define _CONFIGURACIONES

#include "lpc17xx.h"
#include "math.h"
#include "variables.h"

#define Fpclk 25e6 					//Frecuencia de reloj de 25MHz
#define T_muestreo 0.02 		//Periodo de muestreo del TIM1 de 20ms
#define FPWM 1000						//Frecuencia de la PWM de 1kHz


void config_TIMER1(void);
void config_TIMER2(void);
void config_TIMER3(void);
void init_pwm(void);
void init_odom(void);
void configuracion_parametros_mecanicos (param_mecanicos *mecanica, cinematica *variable);

#endif
