
/*****************************************
**************** setup.c  ****************

 Función setup_motor_x() con la configuración del motor del eje x y sus finales de carrera
 Función setup_motor_z() con la configuración del motor del eje z y sus finales de carrera
 Función setup_evalvulas() con la configuración de las 5 electrovalvulas y el generador de vacío
*****************************************/

#include "lpc17xx.h"
#include "constants.h"

void setup_motor_x() {
	LPC_GPIO1->FIODIR |= 1<<PIN_MOTOR_X_STEP; // Definido como salida
	LPC_SC->PCLKSEL0 |= 1<<4;					// PCLK_TIMER1 = CCLK(100MHz)
	LPC_SC->PCONP |= 1<<2; 						// Power up Timer1
	LPC_PINCON->PINSEL3 |= 3<<18;			// P1.25 como MAT1.1
	LPC_TIM1->EMR |= (3<<6);					// External Match 1 toggle
	LPC_TIM1->MCR |= 3<<3;	 					// Match 1 interrumpe y resetea
	LPC_TIM1->MR1 = PASOS_PERIODO/2;	// Periodo/2
	NVIC_EnableIRQ(TIMER1_IRQn);
}

void setup_motor_y() {
	LPC_GPIO0->FIODIR |= 1<<PIN_MOTOR_Z_STEP; // Definido como salida
	LPC_SC->PCLKSEL0 |= 1<<2;					// PCLK_TIMER0 = CCLK(100MHz)
	LPC_SC->PCONP |= 1<<1; 						// Power up Timer0
	LPC_PINCON->PINSEL7 |= 2<<20;			// P3.26 como MAT0.1
	LPC_TIM0->EMR |= (3<<6);					// External Match 1 toggle
	LPC_TIM0->MCR |= 3<<3;	 					// Match 1 interrumpe y resetea
	LPC_TIM0->MR1 = PASOS_PERIODO/2;	// Periodo/2
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void setup_i2c() {
	LPC_PINCON->PINSEL0 |= 3<<SDA; //P0.0 en modo SDA1
	LPC_PINCON->PINSEL0 |= 3<<SCL; //P0.1 en modo SCL1
}
