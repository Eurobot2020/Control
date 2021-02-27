#ifndef _ODOMETRIA
#define _ODOMETRIA
#include "lpc17xx.h"
#include "variables.h"

extern Caracterizacion Robot;			//Por ahora los valores se meten en pose pero mas adelante cambiar para que vayan aqui directos


typedef struct posicion				// definimos la estructura posicion, que contiene los valores de cordenada x e y y angulo del robot,
															// además de los valores de los contadores de los encoders
{
	double pos_x, pos_y, theta;
	int contador_derecho, contador_izquierdo;
} posicion;

posicion pose;								//Variable global que contiene los valores de encoder y posicion que se pasarán a alto nivel

void init_odom(void);									//inicializacion de la odometria. Configura las EINT y las GPIO necesarias para medir en
																			//cuadratura y habilita las interrupciones.
void act_odom(posicion pos_actual);		//actualiza la odometria en una estructura posicion dada. Hay que cambiarlo para
																			//que meta los datos en una estructura Caracterizacion.

#endif