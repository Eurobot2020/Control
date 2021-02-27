#ifndef _FUNCIONES_CONTROL
#define _FUNCIONES_CONTROL
#include <Arduino.h>
#include "math.h"
#include "variables.h"
#include "funciones_pwm.h"

void calcula_parametros_freno_emergencia(cinematica *variable, param_mecanicos *mecanica);
void calculo_de_frenada(cinematica *variable,param_mecanicos *mecanica);
void calcula_parametros_recta (cinematica *variable, param_mecanicos *mecanica);
void calcula_parametros_giro (cinematica *variable, param_mecanicos *mecanica);
void calcula_error_rueda_derecha (cinematica *variable, param_mecanicos *mecanica);
void calcula_error_rueda_izquierda (cinematica *variable, param_mecanicos *mecanica);
void Ajustar_distancia_giro (cinematica *variable,param_mecanicos *mecanica);
void Ajustar_distancia_recta (cinematica *variable,param_mecanicos *mecanica);
void calcula_error_rueda_derecha_final (cinematica *variable, param_mecanicos *mecanica);
void calcula_error_rueda_izquierda_final (cinematica *variable, param_mecanicos *mecanica);



#endif
