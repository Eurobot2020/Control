#ifndef _VARIABLES
#define _VARIABLES

#include <Arduino.h>


/****  Variables odometria  ****/
#define 	AVANZA					1
#define 	RETROCEDE				0

extern uint8_t fl_fin_timer;
extern long contador;
extern char Request;

typedef struct								//Estructura para los parametros de la cinematica
{
	double 			distancia_total_rad,			//distancia total a recorrer que se nos pasa desde alto nivel convertida a radianes
					distancia_frenada,  			//distancia recorrida durante el tiempo de deceleración convertida a radianes
					distancia_acel_vel_cte,		//distancia que debe recorrer cuando acelera y va a velocidad constante/distancia recorrida cuando la velocidad es constante convertida a radianes
	
					error_posicion_actual_derecha,
					error_posicion_actual_izquierda,
	
					error_posicion_actual_derecha_total,
					error_posicion_actual_izquierda_total,
	
					distancia,
					angulo,
					ajustar_distancia,
	
					velocidad_inicial,	//velocidad inicial en rad/s
					velocidad_final;		//velocidad final en rad/s
	
	
}cinematica;

typedef struct								//Estructura para los parametros mecanicos (motor + controladora)
{
	double aceleracion,					//Aceleraci�n de la controladora de 3000 rpm/s
				 deceleracion,				//Valor de deceleracion de los motores
				 reductora,					//Reduccion de velocidad de 26
				 vel_eje_max,					//Velocidad m�xima del eje del motor segun datasheet
				 vel_max,							//Velocidad m�xima de la rueda
				 pulsos_por_rev,			//Pulsos por revolucion del encoder
				 L,							//Longitud del eje
				 diametro;						//Diametro de la rueda
	
}param_mecanicos;

/**** Caracterizaci�n del robot ****/

typedef struct {

	double X;																				// Coordenada X del robot en mm
	double Y;																				// Coordenada Y del robot en mm

}Posicion;

typedef struct {
	
	Posicion Pos;																	// Coordenadas actuales del robot
	double Orientacion;															// �ngulo con respecto a la orientaci�n del campo
	double VelActual;																// Velocidad actual del robot en mm/s
	
}Caracterizacion;

typedef struct 				
											// definimos los valores de los contadores de los encoders
{
	long contador_derecho, contador_izquierdo;
	long contador_derecho_total, contador_izquierdo_total;
	long contador_derecho_total_sumado, contador_izquierdo_total_sumado;
	int cabeceo;			//0 = recta; 1 = giro I(da igual el sentido porque lo negamos el encoder)
} T_Counter;			//Variable global que contiene los valores de posicion que se pasar�n a alto nivel

#endif
