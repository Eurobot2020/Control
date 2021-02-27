#include <Arduino.h>
#include "math.h"
#include "variables.h"
#include "funciones_pwm.h"
#include "funciones_desplazamiento.h"
#include "stdbool.h"



extern bool vel_fin_cambiar;

extern int distancia;
extern int velocidad_final;
extern int velocidad_maxima ;
extern int radio;
extern int grados_giro;

extern T_Counter cuadratura;

void calcula_parametros_freno_emergencia(cinematica *variable, param_mecanicos *mecanica){

// la formula es d=|(v_fin^2-v_ini^2)/2a| donde v_fin vale 0 y la v_ini es la v_fin del anterior
	variable->velocidad_inicial = variable->velocidad_final;
	variable->velocidad_final = 0;
	
	variable->distancia_frenada = fabs ( pow(variable->velocidad_final,2) - pow(variable->velocidad_inicial,2) ) 
		/ ( 2 * mecanica->deceleracion * PI / 30 * ( 1 / mecanica->reductora ) );
	variable->velocidad_final = 0;
	variable->distancia_total_rad = variable->distancia_frenada;
	
	variable->error_posicion_actual_derecha_total = 100;
	variable->error_posicion_actual_izquierda_total = 100;
}


// Esta funcion calcula la distancia recorridad durante el periodo de frenado---------------------------------------------
void calculo_de_frenada(cinematica *variable,param_mecanicos *mecanica)		
{
	variable->velocidad_inicial = 0;			//ELIMINAR CUANDO NO EMPIECE PARADO
	
	// la formula es d=|(v_fin^2-v_ini^2)/2a| donde v_fin vale 0 y la v_ini es la v_fin del anterior
	variable->distancia_frenada = fabs ( pow(variable->velocidad_final,2) - pow(variable->velocidad_inicial,2) ) 
		/ ( 2 * mecanica->deceleracion * PI / 30 * ( 1 / mecanica->reductora ) );
	// Serial.println(variable->distancia_total_rad);
	// Serial.println(variable->distancia_frenada);
	//Debemos hacer una comprobacion por si la distancia de frenado es superior al 50 % de la distancia_total_rad
	if(variable->distancia_frenada > 0.5 * (variable->distancia_total_rad))
	{
		//Ponemos el flag de tipo bool vel_fin_cambiar a true para despues calcular una nueva velocidad final y una nueva
		//deceleracion para este caso. Despues los calculos se har�n para que la distancia de frenado sea el 50 % de 
		//la distancia total a recorrer en radianes
		vel_fin_cambiar = true;
	}	
	else
	{
		vel_fin_cambiar = false;
	}		
}
void Ajustar_distancia_giro (cinematica *variable,param_mecanicos *mecanica)
{
	uint8_t caso;
			
		caso = 1 + (fabs(variable->angulo) > 15) + (fabs(variable->angulo) > 30) + (fabs(variable->angulo) > 60) +
		(fabs(variable->angulo) > 90) + (fabs(variable->angulo) > 120) + (fabs(variable->angulo) > 150);
	
		switch (caso)
		{
			case 1:
				variable->ajustar_distancia = 0.005;
			break;
		
			case 2:
				variable->ajustar_distancia = 0.01;
			break;
		
			case 3:
				variable->ajustar_distancia = 0.015;
			break;
		
			case 4:
				variable->ajustar_distancia = 0.08;
			break;
		
			case 5:
				variable->ajustar_distancia = 0.1;
			break;
		
			case 6:
				variable->ajustar_distancia = 0.15;
			break;
		
			case 7:
				variable->ajustar_distancia = 0.25;
			break;
			
		}
	}
	
void Ajustar_distancia_recta (cinematica *variable,param_mecanicos *mecanica){

	uint8_t caso;
	double vel_real = (double) (variable->velocidad_final * mecanica->diametro * 10 / 2);

		
		caso = 1 + (fabs(vel_real) > 50) + (fabs(vel_real) > 100) + (fabs(vel_real) > 250) +
		(fabs(vel_real) > 350) + (fabs(vel_real) > 550) + (fabs(vel_real) >= 600);
	
		switch (caso)
		{
			case 1:
				variable->ajustar_distancia = 0.001;
			break;
		
			case 2:
				variable->ajustar_distancia = 0.001;
			break;
		
			case 3:
				variable->ajustar_distancia = 0.001;
			break;
		
			case 4:
				variable->ajustar_distancia = 0.001;
			break;
		
			case 5:
				variable->ajustar_distancia = 0.001;
			break;
		
			case 6:
				variable->ajustar_distancia = 0.001;
			break;
		
			case 7:
				variable->ajustar_distancia = 0.001;
			break;
			
		}
		// Serial.println(vel_real);
		// Serial.println("Ajustando distancia");
		// Serial.println(variable->ajustar_distancia,4);
	}
	
//Funcion que calcula los parametros de la cinematica, dicha funcion se llamara cuando tenga que avanzar o hacer un giro--
void calcula_parametros_recta (cinematica *variable, param_mecanicos *mecanica)
{	
	variable->distancia_total_rad=	fabs(distancia/10)/(mecanica->diametro/2);					//Pasamos a radianes
	variable->velocidad_final =  fabs(velocidad_final/10)/(mecanica->diametro/2);				//Pasamos a rad/s
	// Serial.println(variable->velocidad_final);

	if(distancia>0)
	{
		sentido_motores(AVANZA,AVANZA);
	}
	else
	{
		sentido_motores(RETROCEDE,RETROCEDE);
	}


	variable->error_posicion_actual_derecha = 100;											//Le asigno un valor mayor que la comprobacion del if del bucle
	variable->error_posicion_actual_izquierda = 100;										//Le asigno un valor mayor que la comprobacion del if del bucle
	
	variable->error_posicion_actual_derecha_total = 100;
	variable->error_posicion_actual_izquierda_total = 100;
	
	//Calcular la distancia que tiene que recorrer cuando acelere y vaya a velocidad constante
	calculo_de_frenada(variable,mecanica);
	variable->distancia_acel_vel_cte = variable->distancia_total_rad - variable->distancia_frenada; 
	
	if(vel_fin_cambiar)	//Si resulta que hay que cambiar la velocidad ejecutamos esto
			{
				//Calculamos la nueva velocidad que habria que meterle a las PWM
				variable->distancia_frenada = 0.5 * variable->distancia_total_rad;
				variable->velocidad_final = sqrt( ( ( 2 * mecanica->deceleracion * PI / 30 * ( 1 / mecanica->reductora ) ) 
					* variable->distancia_frenada) + pow(variable->velocidad_inicial,2));
				//Asignamos el nuevo valor a distancia_acel_vel_cte que ser� el 50 % de la distancia total
				variable->distancia_acel_vel_cte = 0.5 * variable->distancia_total_rad;
			}
	Ajustar_distancia_recta(variable,mecanica);
	Serial.println(variable->distancia_total_rad);
	Serial.println(variable->distancia_acel_vel_cte);
}


void calcula_parametros_giro (cinematica *variable, param_mecanicos *mecanica)
{		
	variable->distancia_total_rad = ( PI * fabs(grados_giro)) * mecanica->L / (360 * ( mecanica->diametro /2 ) );
	variable->velocidad_final = 	2.5;														//	5;
	Serial.println(variable->distancia_total_rad);

	calculo_de_frenada(variable,mecanica);
	variable->distancia_acel_vel_cte = variable->distancia_total_rad - variable->distancia_frenada; 
	
	if(vel_fin_cambiar)	//Si resulta que hay que cambiar la velocidad ejecutamos esto
			{
				//Calculamos la nueva velocidad que habria que meterle a las PWM
				variable->distancia_frenada = 0.5 * variable->distancia_total_rad;
				variable->velocidad_final = sqrt( ( ( 2 * mecanica->deceleracion * PI / 30 * ( 1 / mecanica->reductora ) ) 
					* variable->distancia_frenada) + pow(variable->velocidad_inicial,2));
				//Asignamos el nuevo valor a distancia_acel_vel_cte que ser� el 50 % de la distancia total
				variable->distancia_acel_vel_cte = 0.5 * variable->distancia_total_rad;
			}
			
	if(grados_giro>0)
		{
			sentido_motores(RETROCEDE,AVANZA);
		}
		else
		{
			sentido_motores(AVANZA,RETROCEDE);
		}

	variable->error_posicion_actual_derecha = 100;											//Le asigno un valor mayor que la comprobacion del if del bucle
	variable->error_posicion_actual_izquierda = 100;										//Le asigno un valor mayor que la comprobacion del if del bucle
	
	variable->error_posicion_actual_derecha_total = 10;
	variable->error_posicion_actual_izquierda_total = 10;
	Ajustar_distancia_giro(variable,mecanica);
}

void calcula_error_rueda_derecha (cinematica *variable, param_mecanicos *mecanica)
{
	variable->error_posicion_actual_derecha = variable->distancia_acel_vel_cte - ( ( (cuadratura.contador_derecho_total) * 2 * PI ) /
	( mecanica->pulsos_por_rev * mecanica->reductora ) );
	//distancia_acel_vel_cte
}

void calcula_error_rueda_izquierda (cinematica *variable, param_mecanicos *mecanica)
{
	variable->error_posicion_actual_izquierda = variable->distancia_acel_vel_cte - ( ( (cuadratura.contador_izquierdo_total) * 2 * PI ) /
	( mecanica->pulsos_por_rev * mecanica->reductora ) );
	//distancia_acel_vel_cte
}
void calcula_error_rueda_derecha_final (cinematica *variable, param_mecanicos *mecanica)
{
	variable->error_posicion_actual_derecha_total = variable->distancia_total_rad - ( ( (cuadratura.contador_derecho_total) * 2 * PI ) /
	( mecanica->pulsos_por_rev * mecanica->reductora ) );
}

void calcula_error_rueda_izquierda_final (cinematica *variable, param_mecanicos *mecanica)
{
	variable->error_posicion_actual_izquierda_total = variable->distancia_total_rad - ( ( (cuadratura.contador_izquierdo_total) * 2 * PI ) /
	( mecanica->pulsos_por_rev * mecanica->reductora ) );
}
