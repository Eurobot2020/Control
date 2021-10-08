#include "math.h"
//defines PWM


#include <Arduino.h>

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


//FUNCION PWM2_SetDuty: Esta funci�n cambia la velocidad de la rueda derecha y acepta valores entre 800 y -800 (las controladoras
//MAXON aceptan un rango de pwm de 10% a 90% con una resoluci�n de 0.1% lo que hace 800 valores posibles en cada sentido) 
void PWM2_SetDuty (float ciclo)
{
	float duty;																										//Se define la variable duty, que sirve para calcular de forma
																																//sencilla el valor del MR2
	//Serial.println(ciclo);
	duty=(float)fabs((double)ciclo) * 0.8	+	0.099;					//El duty se calcula como el 10% de m�nimo mas el valor absoluto de
																																//la velocidad entre mil. La funci�n fabs recibe y devuelve un double
																																//y es por eso por lo que necesitamos las conversiones de tipo.
	OCR0B = duty*255;
	//Serial.println(duty);
	return;
}
//FUNCION PWM3_SetDuty: Esta funci�n cambia la velocidad de la rueda izquierda y acepta valores entre 800 y -800 (las controladoras
//MAXON aceptan un rango de pwm de 10% a 90% con una resoluci�n de 0.1% lo que hace 800 valores posibles en cada sentido) 
void PWM3_SetDuty (float ciclo)
{
	float duty;																										//Se define la variable duty, que sirve para calcular de forma
																																//sencilla el valor del MR3
	// Serial.println(ciclo);
	duty=(float)fabs((double)ciclo)	* 0.8	+	0.099;			//El duty se calcula como el 10% de m�nimo mas el valor absoluto de
																																//la velocidad entre mil. La funci�n fabs recibe y devuelve un double
																																//y es por eso por lo que necesitamos las conversiones de tipo.
	OCR0A = duty*255;
	//Serial.println(duty);	
	return;
}
