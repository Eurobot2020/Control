#include <Arduino.h>
#include "math.h"
#include "funciones_pwm.h"
#include "variables.h"
#include "funciones_desplazamiento.h"
#include "funciones_control.h"

extern cinematica lazo_abierto;
extern param_mecanicos maxon;
extern Caracterizacion Robot;
extern T_Counter cuadratura;

extern long contador;
extern bool sentido_dcha;
extern bool sentido_izq;
extern bool parado;
void act_odom(Caracterizacion *situacion, T_Counter *contadores, param_mecanicos *mecanica) 
{
	double deltaO, avance, deltaX, deltaY;		//definici�n de avance en recto del robot, incremento del �ngulo, incremento en X e incremento en Y 

	double p_a_mm = mecanica->diametro*10*PI/(mecanica->pulsos_por_rev*mecanica->reductora);


	//avance=(contadores->contador_derecho*p_a_mm	 +	contadores->contador_izquierdo*p_a_mm)	/	2;          				//c�lculo del avance en recto del robot
	//deltaO=((contadores->contador_derecho*p_a_mm  -	 contadores->contador_izquierdo*p_a_mm)	 /  mecanica->L*10 ) * 180 / PI;		//c�lculo del incremento del �ngulo en grados
	
	avance=(contadores->contador_derecho*p_a_mm	+	contadores->contador_izquierdo*p_a_mm)	/	2;          						//c�lculo del avance en recto del situacion
	deltaO=((contadores->contador_derecho*p_a_mm	-	contadores->contador_izquierdo*p_a_mm)	/	(mecanica->L*10))*180/PI;		//c�lculo del incremento del �ngulo en grados
	
	contadores->contador_derecho_total_sumado+=contadores->contador_derecho;
	contadores->contador_izquierdo_total_sumado+=contadores->contador_izquierdo;
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

void check_mov(T_Counter *contadores){
	static long contador_reg_dcha = 0;
	static long contador_reg_izq = 0;
	if (contador_reg_dcha == contadores->contador_derecho_total || contador_reg_izq == contadores->contador_izquierdo_total)
	{
		parado = true;

	}
	else
	{
		parado = false;
	}
	contador_reg_dcha = contadores->contador_derecho_total;
	contador_reg_izq = contadores->contador_izquierdo_total;
}

ISR(TIMER1_COMPA_vect)    //This is the interrupt request
{	
	calcula_error_rueda_izquierda (&lazo_abierto, &maxon);
	calcula_error_rueda_derecha (&lazo_abierto, &maxon);

	calcula_error_rueda_derecha_final (&lazo_abierto, &maxon);
	calcula_error_rueda_izquierda_final (&lazo_abierto, &maxon);
	
	act_odom(&Robot,&cuadratura,&maxon);
	
	check_mov(&cuadratura);
	contador++;

	if (contador%50==0){
		Serial.print("X");
		Serial.print(int(Robot.Pos.X));
		Serial.print("Y");
		Serial.print(int(Robot.Pos.Y));
		Serial.print("A");
		Serial.println(int(Robot.Orientacion));
		// if (parado)
		// {
		// 	Serial.println("PARADO");
		// }
	}
	

	
}


/* **********************************************************************************************************************************
												IMPORTANTE COMPROBAR EL SENTIDO DE LAS RUEDAS
													PARA LAS INTERRUPCIONES DE LOS ENCODERS
***********************************************************************************************************************************/
void int_derecha (void){			//C�digo de la interrupci�n del encoder de la rueda derecha 
	cuadratura.contador_derecho_total++;
	//cuadratura.contador_derecho++;
	// Serial.println("Encoder derecho");
		

		if (!sentido_dcha) 				//Valor del pin 10
		{
			cuadratura.contador_derecho--;		//Si al cambiar a alto el canal A el canal B esta en alto vamos hacia alante
			// Serial.println("Encoder derecho");
			//Serial.println("-");
		}
		else
		{
			cuadratura.contador_derecho++;											//Si al cambiar a alto el canal A el canal B esta en bajo vamos hacia atras
			//Serial.println("+");
		}

		/* if ((PINB&B00000100)>>2==1) 				//Valor del pin 10
		{
			cuadratura.contador_derecho--;		//Si al cambiar a alto el canal A el canal B esta en alto vamos hacia alante
			// Serial.println("Encoder derecho");
			//Serial.println("-");
		}
		else
		{
			cuadratura.contador_derecho++;											//Si al cambiar a alto el canal A el canal B esta en bajo vamos hacia atras
			//Serial.println("+");
		} */
		
}

void int_izquierda (void){			//C�digo de la interrupci�n del encoder de la rueda izquierda 
	cuadratura.contador_izquierdo_total++;
	//cuadratura.contador_izquierdo++;
	// Serial.println("Encoder izquierda");
	
	if (sentido_izq)					//Valor del pin 13
	{
		cuadratura.contador_izquierdo++;											//Si al cambiar a alto el canal A el canal B esta en alto vamos hacia alante
		// Serial.println("Encoder izquierda");
	}
	else
	{
		cuadratura.contador_izquierdo--;											//Si al cambiar a alto el canal A el canal B esta en bajo vamos hacia atras
	}

 	/* if ((PINB&B00100000)>>5==1)					//Valor del pin 13
	{
		cuadratura.contador_izquierdo++;											//Si al cambiar a alto el canal A el canal B esta en alto vamos hacia alante
		// Serial.println("Encoder izquierda");
	}
	else
	{
		cuadratura.contador_izquierdo--;											//Si al cambiar a alto el canal A el canal B esta en bajo vamos hacia atras
	} */ 
	
}
