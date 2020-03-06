
/*****************************************
**************** main.c  *****************

 Funci�n main() con los SetUp necesarios
*****************************************/

#include "LPC17xx.h"
#include "setup.h"
#include "state.h"
#include "GLCD.h"
#include "TouchPanel.h"

int main(){
	setup_motor_x();
	setup_motor_y();
	setup_i2c();
	TP_Init(); 
	LCD_Initializtion();
	
	//TouchPanel_Calibrate();
	
	init_stateMachine(); // Inicializaci�n de la m�quina de estados
	
	while(1) {
		stateMachine();
	}
}
