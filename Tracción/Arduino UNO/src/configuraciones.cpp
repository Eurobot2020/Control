#include <Arduino.h>
#include <math.h>
#include "variables.h"
#include "funciones_timer.h"
#include "PinChangeInterrupt.h"

#define T_muestreo 0.02 		//Periodo de muestreo del TIM1 de 20ms
#define FPWM 1000						//Frecuencia de la PWM de 1kHz

// Pines de controladoras Maxon Derecha OCR0B
#define D_STOP 2
#define D_DIR 3
#define D_EN  4
#define D_PWM 5
// Pines de controladoras Maxon Izquierda OCR0A
#define I_PWM 6
#define I_EN  7
#define I_DIR 8
#define I_STOP 9
// Pines de encoder de motores
#define Enc_I 13
#define PCInt_I 12
#define PCInt_D 11
#define Enc_D 10

/*En este fichero configuraciones.c se escribir�n las funciones para inicializar	
todos los perif�ricos a usar y todos los par�metros iniciales del motor y 
controladora*/

//CONFIGURACI�N DE LOS TIMER----------------------------------------------------------------------------------------------------------
void config_TIMER0(void) //Timer de Fast PWM 1 Khz
{

	TCCR0A = (1<<COM0A1) | (1<<COM0B1) | (1<<WGM01) | (1<<WGM00);    //Set the Fast PWM mode on both channels 
	TCCR0B = (1<<CS01) | (1<<CS00);    		//Set the prescale 1/64 clock
	OCR0A = 15; 								//Reset OCR0x value for safety 
	OCR0B = 15; 								//Reset OCR0x value for safety 
	// TIMSK0 |= (1<<OCIE0A) | (1<<OCIE0B);   	//Set both interrupt requests
	// sei(); 									//Enable interrupt
	pinMode(D_PWM, OUTPUT);					//Enable output
	pinMode(I_PWM, OUTPUT);					//Enable output
	pinMode(D_DIR,OUTPUT);
	pinMode(I_DIR,OUTPUT);
	pinMode(D_EN,OUTPUT);
	pinMode(I_EN,OUTPUT);
	
}

void config_TIMER1(void) //Timer medida tiempos largos con Ts = 20 ms
{
	TCCR1A = 0;                //limpia registros
 	TCCR1B = 0;                //limpia registros
 	TCNT1  = 0;                //Inicializa el contador
	OCR1A = 39999;
	TCCR1B = (1<<WGM12) | (1<<CS11);
	TIMSK1|=(1<<OCIE1A);   //Set the interrupt request
}

void init_odom(void){					//Esta funci�n inicializa las interrupciones externas y las GPIO que llevan los encoders en cuadratura.
	pinMode(Enc_D,INPUT);
	pinMode(Enc_I,INPUT);
	pinMode(PCInt_D,INPUT);
	pinMode(PCInt_I,INPUT);

	attachPCINT(digitalPinToPCINT(PCInt_D), int_derecha, RISING);
	attachPCINT(digitalPinToPCINT(PCInt_I), int_izquierda, RISING);
}

//CONFIGURACION MECANICA DE MOTOR+CONTROLADORA----------------------------------------------------------------------------------------
void configuracion_parametros_mecanicos (param_mecanicos *mecanica,cinematica *variable)
{
	mecanica->aceleracion = 3000;
	mecanica->reductora = 25; // 26 * 0.971;
	mecanica->diametro = 5.45;
	mecanica->vel_eje_max = 8100 / mecanica->reductora;																										//	6380;
	mecanica->vel_max = mecanica->vel_eje_max * PI / 30;
	mecanica->pulsos_por_rev = 256;
	mecanica->L = 9.8;
	
	variable->velocidad_final = 0;																								//Velocidad final que queremos que alcancen los motores siempre
	variable->velocidad_inicial = 0;	
	//Formula que calcula la deceleracion, se ha obtenido de forma experimental calculando el error que cometia cuando le metia una velocidad y le decia que avanzara 1m, de esta forma 
	//experimental sacaba varias distancias de frenado y hacia una media para luego sacar la deceleracion. Este proceso lo repet� en varias ocasiones, cambiando la velocidad y las distancias
	//para mi asombro si establecia una velocidad e iba variando las distancias a recorrer la deceleracion para esa velocidad apenas variaba, pero si cambiaba la velocidad final la deceleracion
	//si que experimentaba un cambio, asi que usando matlab repeti el experimento para una distancia de 1m e iba variando las velocidades, despues obtenia la deceleracion, ponia 
	//decelacion vs velocidad y le aplicaba una aproximacion matematica con una de las herramientas de matlab para sacar una ecuacion aproximada del comprotamiento de la aceleracion ante variaciones
	//de velocidad
	mecanica->deceleracion = 3000;										//( -0.59*pow(variable->velocidad_final,3) + 5.4*pow(variable->velocidad_final,2) - 14*variable->velocidad_final + 17)*(mecanica->reductora)*(1/(2*PI))*60;
}

