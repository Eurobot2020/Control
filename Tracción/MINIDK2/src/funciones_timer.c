#include "lpc17xx.h"
#include "math.h"
#include "funciones_pwm.h"
#include "variables.h"
#include "funciones_desplazamiento.h"
#include "funciones_control.h"
#define Fpclk 25e6
#define T_muestreo 0.02 

extern cinematica lazo_abierto;
extern param_mecanicos maxon;
extern Caracterizacion Robot;
extern T_Counter cuadratura;

extern int contador;

void act_odom(Caracterizacion *situacion, T_Counter *contadores, param_mecanicos *mecanica) 
{
	double deltaO, avance, deltaX, deltaY;		//definici�n de avance en recto del robot, incremento del �ngulo, incremento en X e incremento en Y 

	double p_a_mm = mecanica->diametro*10*PI/(mecanica->pulsos_por_rev*mecanica->reductora);


	//avance=(contadores->contador_derecho*p_a_mm	 +	contadores->contador_izquierdo*p_a_mm)	/	2;          				//c�lculo del avance en recto del robot
	//deltaO=((contadores->contador_derecho*p_a_mm  -	 contadores->contador_izquierdo*p_a_mm)	 /  mecanica->L*10 ) * 180 / PI;		//c�lculo del incremento del �ngulo en grados
	
	avance=(contadores->contador_derecho*p_a_mm	+	contadores->contador_izquierdo*p_a_mm)	/	2;          						//c�lculo del avance en recto del situacion
	deltaO=((contadores->contador_derecho*p_a_mm	-	contadores->contador_izquierdo*p_a_mm)	/	(mecanica->L*10))*180/PI;		//c�lculo del incremento del �ngulo en grados
	
	
	contadores->contador_derecho    = 0;
	contadores->contador_izquierdo 	= 0;

	deltaX						=		avance*cos((situacion->Orientacion+deltaO)*PI/180);   			//calculo del avance en X del situacion  (funcion cos en radianes)
	deltaY						=		avance*sin((situacion->Orientacion+deltaO)*PI/180);				//calculo del avance en Y del situacion  (funcion sin en radianes)
	
	situacion->Pos.X				+=	deltaX;
	situacion->Pos.Y				+=	deltaY;						//incrementar las variables globales con los datos calculados
	situacion->Orientacion 		+=	deltaO;

	while (situacion->Orientacion>=360)							// si se pasa de 360� convierte a rango 0
		situacion->Orientacion-=360;								// no se contempla que de mas de una vuelta en un tiempo de muestreo
	while (situacion->Orientacion<0)									// si se pasa de 360� convierte a rango 0
		situacion->Orientacion+=360;	

}

void TIMER1_IRQHandler(void)
{
	
	LPC_TIM1->IR|=(1<<1); // borro el flag de la interrupcion
	
	calcula_error_rueda_izquierda (&lazo_abierto, &maxon);
	calcula_error_rueda_derecha (&lazo_abierto, &maxon);

	calcula_error_rueda_derecha_final (&lazo_abierto, &maxon);
	calcula_error_rueda_izquierda_final (&lazo_abierto, &maxon);
	
	act_odom(&Robot,&cuadratura,&maxon);
	
	contador++;
}

/***********************************************************************************************************************************
																				IMPORTANTE COMPROBAR EL SENTIDO DE LAS RUEDAS
																					PARA LAS INTERRUPCIONES DE LOS ENCODERS
***********************************************************************************************************************************/
void EINT2_IRQHandler (void){			//C�digo de la interrupci�n del encoder de la rueda derecha 
	LPC_SC->EXTINT = (1 << 2);
	cuadratura.contador_derecho_total++;
	
		if ((LPC_GPIO0->FIOPIN >> 11/*23*/) & 0x1)			//Valor del pin 0.23/0.11
		{
			cuadratura.contador_derecho--;		//Si al cambiar a alto el canal A el canal B esta en alto vamos hacia alante
		}
		else
		{
			cuadratura.contador_derecho++;												//Si al cambiar a alto el canal A el canal B esta en bajo vamos hacia atras
		}

}

void EINT3_IRQHandler (void){			//C�digo de la interrupci�n del encoder de la rueda izquierda 
	LPC_SC->EXTINT = (1 << 3);
	cuadratura.contador_izquierdo_total++;
	
	if ((LPC_GPIO0->FIOPIN >> 10/*4*/) & 0x1)					//Valor del pin 0.4/0.10
	{
		cuadratura.contador_izquierdo--;											//Si al cambiar a alto el canal A el canal B esta en alto vamos hacia alante
	}
	else
	{
		cuadratura.contador_izquierdo++;											//Si al cambiar a alto el canal A el canal B esta en bajo vamos hacia atras
	}

}



