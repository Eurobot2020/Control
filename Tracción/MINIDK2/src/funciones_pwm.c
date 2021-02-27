#include "lpc17xx.h"
#include "math.h"
//defines PWM





//FUNCION PWM2_SetDuty: Esta funci�n cambia la velocidad de la rueda derecha y acepta valores entre 800 y -800 (las controladoras
//MAXON aceptan un rango de pwm de 10% a 90% con una resoluci�n de 0.1% lo que hace 800 valores posibles en cada sentido) 
void PWM2_SetDuty (float velocidad)
{
	float duty;																										//Se define la variable duty, que sirve para calcular de forma
																																//sencilla el valor del MR2
	
	duty=(float)fabs((double)velocidad)	/	1000	+	0.1;						//El duty se calcula como el 10% de m�nimo mas el valor absoluto de
																																//la velocidad entre mil. La funci�n fabs recibe y devuelve un double
																																//y es por eso por lo que necesitamos las conversiones de tipo.
	
	LPC_PWM1->MR2				 = 	(uint32_t)(duty*LPC_PWM1->MR0);				//Se calcula el MR2 como el duty por el valor de MR0 convertido a uint32_t
	
	LPC_PWM1->LER 			|= 	(1<<2);																//Se actualiza el valor de MR2 desde el registro LER
	/*
	if(distancia>=0)
		LPC_GPIO3->FIOCLR	|=	(1<<25);															//Si la velocidad es positiva ponemos el bit de sentido derecho a 0 (avance)
	
	else
		LPC_GPIO3->FIOSET	|=	(1<<25);															//En caso contrario se pone a 1(retroceso)
	*/
	//LPC_GPIO2->FIOSET1	|=	(1<<2);																//Se pone a 1 el bit de enable derecho(2.10).
	/*
	if (velocidad == 0)
	LPC_GPIO2->FIOCLR1	|=	(1<<2);																//En el caso de velocidad cero desactivamos tambien el enable.
	*/
	return;
}
//FUNCION PWM3_SetDuty: Esta funci�n cambia la velocidad de la rueda izquierda y acepta valores entre 800 y -800 (las controladoras
//MAXON aceptan un rango de pwm de 10% a 90% con una resoluci�n de 0.1% lo que hace 800 valores posibles en cada sentido) 
void PWM3_SetDuty (float velocidad)
{
	float duty;																										//Se define la variable duty, que sirve para calcular de forma
																																//sencilla el valor del MR3
	
	duty	=		(float)fabs((double)velocidad)	/	1000	+	0.1;			//El duty se calcula como el 10% de m�nimo mas el valor absoluto de
																																//la velocidad entre mil. La funci�n fabs recibe y devuelve un double
																																//y es por eso por lo que necesitamos las conversiones de tipo.
	
	
	
	LPC_PWM1->MR3	 			 = 	(uint32_t)(duty*LPC_PWM1->MR0);				//Se calcula el MR3 como el duty por el valor de MR0 convertido a uint32_t
	
	LPC_PWM1->LER 			|=	(1<<3);																//Se actualiza el valor de MR3 desde el registro LER
	/*
	if(distancia>=0)
		LPC_GPIO3->FIOCLR	|=	(1<<26);															//Si la velocidad es positiva ponemos el bit de sentido izquierdo a 0 (avance)
	
	else
		LPC_GPIO3->FIOSET	|=	(1<<26);															//En caso contrario se pone a 1(retroceso)
	*/
	//LPC_GPIO2->FIOSET1	|=	(1<<3);																//Se pone a 1 el bit de enable izquierdo(2.11).
	/*
	if (velocidad == 0)
	LPC_GPIO2->FIOCLR1	|=	(1<<3);																//En el caso de velocidad cero desactivamos tambien el enable.
	*/
	return;
}
