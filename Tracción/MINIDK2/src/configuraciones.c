#include "lpc17xx.h"
#include <math.h>
#include "variables.h"

#define Fpclk 25e6 					//Frecuencia de reloj de 25MHz
#define T_muestreo 0.02 		//Periodo de muestreo del TIM1 de 20ms
#define FPWM 1000						//Frecuencia de la PWM de 1kHz

	
/*En este fichero configuraciones.c se escribir�n las funciones para inicializar	
todos los perif�ricos a usar y todos los par�metros iniciales del motor y 
controladora*/

//CONFIGURACI�N DE LOS TIMER----------------------------------------------------------------------------------------------------------
void config_TIMER1(void)
{
	LPC_SC->PCONP|=(1<<2); 							//Alimento el periferico del TIM1
	LPC_TIM1->MR1=Fpclk*T_muestreo - 1;	//Ajusto el MR1 para que interrumpa cada 20ms
	LPC_TIM1->MCR|=(3<<3); 							//Cuando se produzca un match en el MR1 hago un reset y que interrumpa
	LPC_TIM1->TCR|=1;										//Habilito el TC para que cuente
	NVIC_EnableIRQ(TIMER1_IRQn); 				//Habilito la interrupcion del TIM1
	NVIC_SetPriority(TIMER1_IRQn,1); 		//Prioridad 1 al TIMER
}

//Eliminamos estas funciones al haber sido sustituidas por la semi cuadratura mediante EINT
//En el caso de POSAVASOS hay que asegurarse de conectar los encoders externos
//NO LOS DEL MOTOR MAXON


void config_TIMER2(void)
{
	LPC_SC->PCONP|=(1<<22); 			//Alimento el periferico del TIM2
	LPC_PINCON->PINSEL0|=(3<<8); 	//Hago que la patilla P0.4 se comporte como CAP2.0
	
	
	
	LPC_TIM2->CTCR|=(1<<0); 			//Trabaja en modo Count, solo cuenta pulsos
	LPC_TIM2->CCR&=~(7<<0);				//Me aseguro de tener los tres primeros bits, por ser CAP2.0,	a cero como dice el manual
	LPC_TIM2->TCR=1;							//Hbilito el TC para que empiece a contar
}

void config_TIMER3(void)
{
	LPC_SC->PCONP|=(1<<23); 			//Alimento el periferico del TIM3
	LPC_PINCON->PINSEL1|=(3<<14); //Hago que la patilla P0.23 se comporte como CAP3.0
	

	
	LPC_TIM3->CTCR|=(1<<0); 			//Trabaja en modo Count, solo cuenta pulsos
	LPC_TIM3->CCR&=~(7<<0);				//Me aseguro de tener los tres primeros bits, por ser CAP3.0,	a cero como dice el manual
	LPC_TIM3->TCR=1;							//Hbilito el TC para que empiece a contar
}



//CONFIGURACION DE LAS PWM------------------------------------------------------------------------------------------------------------

//FUNCI�N init_pwm: Inicia y configura todo lo relacionado con las PWM1.2 (que se usa para la rueda derecha) y PWM1.3 (que
//se usa para la rueda izquierda) y los pines de habilitaci�n y sentido de las controladoras MAXON.
//Para poder usar estas PWM hay que poner esta funci�n a ser posible al principio del codigo main().

void init_pwm(void)  
{
	LPC_PINCON->PINSEL3 |=	(2<<8)			|		(2<<10);					//Se configuran lso pines 1.20 y 1.21 como PWM1.2 y PWM1.3 respectivamente.
	
	LPC_PINCON->PINSEL4 |= (1<<2);
	
	LPC_SC->PCONP				|=	(1<<6);														//Se activa la alimentaci�n de las PWM mediante su pin del registro PCONP.
	
	LPC_PWM1->MCR 			= 	(1<<1);														//Se configura para que no se genere interrupci�n pero si se resetee en MR0.
	
	LPC_PWM1->CTCR			|=	(0<<0)			|		(0<<1);	    			//Se configura para que la PWM funcione con su timer.
	
	LPC_PWM1->MR0				=		(Fpclk/FPWM)	-		1;       					//Se ajusta MR0 para la f deseada.	
																														//El -1 es porque la cuenta comienza en 0.
	
	LPC_PWM1->LER				|=	(1<<0)			| 	(1<<2)	| (1<<3);	//Se cargan los valores puestos en los registros de MR0, MR2 y MR3 
																														//siendo estos dos �ltimos 0 por el reset.
	
	LPC_PWM1->PCR				&=	(0<<2)			|		(0<<3);						//Se configuran las PWM1.2 y PWM1.3 como controladas por un �nico flanco 
	
	LPC_PWM1->PCR				|=	(1<<10)			|		(1<<11);					//Se habilitan los canales de salida de los pines de pwm1.2 y pwm1.3
	
	LPC_PWM1->TCR				|=	(1<<1);														//Se resetea el valor del timer
	
	LPC_PWM1->TCR				=		(1<<3)			|		(1<<0);						//Se habilita el contador y la PWM.

	LPC_GPIO2->FIODIR1	|=	(3<<2);														//Se configuran los GPIOS 2.11 y 2.10 como salida para los enable de
																													//la controladora. el 2.11 es para la izquierda y el 2.10 para la derecha.
																																																	
	LPC_GPIO2->FIOCLR1	|=	(3<<2);														//Se ponen los pines a 0 como predeterminado.
		
	LPC_GPIO3->FIODIR	|=	(3<<25);														//Se confguran los pines 3.25 y 3.26 (pines de sentido de las ruedas)
																														//como salidas. el 3.25 corresponde a la derecha y el 3.26 a la izquierda.
																														//Los valores de sentido en Sebastian son 0 para el avance de ambas ruedas.
}

void init_odom(void){					//Esta funci�n inicializa las interrupciones externas y las GPIO que llevan los encoders en cuadratura.
	
	LPC_PINCON->PINSEL4 |= 	(1<<24) | (1<<26) ; //Pongo los pines 2.12 y 2.13 como EINT2 y EINT3 (derecha e izquierda respectivamente).
	LPC_PINCON->PINSEL0 &= ~( (3<<20) | (3<<22) ) ; //Pongo los pines 0.10 y 0.11 como GPIOS.
	LPC_GPIO0->FIODIR 	&= ~( (1<<10) | (1<<11) ) ;	//Defino los pines 0.10 y 0.11 como INPUT.
	
	LPC_SC->EXTMODE			|=	(1<<2)	| (1<<3) ;	//Pongo las interrupciones 2 y 3 para ser sensibles a flanco
	LPC_SC->EXTPOLAR		|=	(1<<2)	| (1<<3) ;  //Defino el flanco de activavi�n como el flanco de subida
	
	NVIC_EnableIRQ(EINT2_IRQn);									//Activo las interrupciones para EINT 2 y 3			
	NVIC_EnableIRQ(EINT3_IRQn);
	NVIC_SetPriority(EINT2_IRQn, 1);						//Ajusto la prioridad de las interruciones a un valor alto
	NVIC_SetPriority(EINT3_IRQn, 2);

}

//CONFIGURACION MECANICA DE MOTOR+CONTROLADORA----------------------------------------------------------------------------------------
void configuracion_parametros_mecanicos (param_mecanicos *mecanica,cinematica *variable)
{
	mecanica->aceleracion = 3000;
	mecanica->reductora = 25.24;
	mecanica->diametro = /*5.2;*/ 12.2;
	mecanica->vel_eje_max = 8100;																										//	6380;
	mecanica->vel_max = mecanica->vel_eje_max * PI / 30 / mecanica->reductora;
	mecanica->pulsos_por_rev = 256;
	mecanica->L = /*9.8;*/	34.5;
	
	variable->velocidad_final = 5;																								//Velocidad final que queremos que alcancen los motores siempre
	variable->velocidad_inicial = 0;	
	//Formula que calcula la deceleracion, se ha obtenido de forma experimental calculando el error que cometia cuando le metia una velocidad y le decia que avanzara 1m, de esta forma 
	//experimental sacaba varias distancias de frenado y hacia una media para luego sacar la deceleracion. Este proceso lo repet� en varias ocasiones, cambiando la velocidad y las distancias
	//para mi asombro si establecia una velocidad e iba variando las distancias a recorrer la deceleracion para esa velocidad apenas variaba, pero si cambiaba la velocidad final la deceleracion
	//si que experimentaba un cambio, asi que usando matlab repeti el experimento para una distancia de 1m e iba variando las velocidades, despues obtenia la deceleracion, ponia 
	//decelacion vs velocidad y le aplicaba una aproximacion matematica con una de las herramientas de matlab para sacar una ecuacion aproximada del comprotamiento de la aceleracion ante variaciones
	//de velocidad
	mecanica->deceleracion = 3000;										//( -0.59*pow(variable->velocidad_final,3) + 5.4*pow(variable->velocidad_final,2) - 14*variable->velocidad_final + 17)*(mecanica->reductora)*(1/(2*PI))*60;
}

