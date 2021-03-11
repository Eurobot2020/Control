#ifndef _FUNCIONES_TIMER
#define _FUNCIONES_TIMER
#include <Arduino.h>
#include "funciones_pwm.h"

#define Fpclk 25e6

void act_odom(Caracterizacion *situacion, T_Counter *contadores, param_mecanicos *mecanica);
void check_mov(Caracterizacion *situacion);
ISR(TIMER1_COMPA_vect);
void int_derecha (void);
void int_izquierda (void);
#endif
