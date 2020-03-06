/*****************************************
**************** state.c  ****************

 Función init_stateMachine() con la inicicialización de la máquina de estados
 Función stateMachine() con la máquina de estados
*****************************************/

#include "LPC17xx.h"
#include "constants.h"
#include "pantalla.h"
#include "GLCD.h"
#include "i2c_lpc17xx.h"

uint8_t state;

int x = 0;
int z = 0;
uint8_t pulsos = 0;
unsigned char actuadores = 0;

void init_stateMachine() {
	state = MAIN;
}

void stateMachine() {
	switch(state)
	{
		case MAIN:
			screenMain();
			state = MAIN_WAIT;
			break;
		case MAIN_WAIT:
			if(zoneNewPressed(&zone_1c) && x>-5) { // Eje X -
				IZQUIERDA;
				MOTOR_X_ON;
				x--;
				squareButton(&zone_1b, (char *)x, Black, Gray, Gray);
				state = PULSO;
			}
			if(zoneNewPressed(&zone_1d) && x<5) { // Eje X +
				DERECHA;
				MOTOR_X_ON;
				x++;
				squareButton(&zone_1b, (char *)x, Black, Gray, Gray);
				state = PULSO;
			}
			if(zoneNewPressed(&zone_2c) && z>-30) { // Eje Z -
				ABAJO;
				MOTOR_Z_ON;
				z--;
				squareButton(&zone_2b, (char *)z, Black, Gray, Gray);
				state = PULSO;
			}
			if(zoneNewPressed(&zone_2d) && z<0) { // Eje Z +
				ARRIBA;
				MOTOR_Z_ON;
				z++;
				squareButton(&zone_2b, (char *)z, Black, Gray, Gray);
				state = PULSO;
			}
			if(zoneNewPressed(&zone_3b)) { // Generador de Vacío 1
				if(actuadores & GV1) { // ON->OFF
					actuadores &= ~GV1;
					I2CSendAddr(ADDR, W);
					I2CSendByte(actuadores);
					I2CSendStop();
					fillRect(&zone_3b, Black);					
				}
				else {								// OFF->ON
					actuadores |= GV1;
					I2CSendAddr(ADDR, W);
					I2CSendByte(actuadores);
					I2CSendStop();
					fillRect(&zone_3b, Green);
				}
			}
			if(zoneNewPressed(&zone_3c)) { // Generador de Vacío 2
				if(actuadores & GV2) { // ON->OFF
					actuadores &= ~GV2;
					I2CSendAddr(ADDR, W);
					I2CSendByte(actuadores);
					I2CSendStop();
					fillRect(&zone_3c, Black);
				}
				else {								// OFF->ON
					actuadores |= GV2;
					I2CSendAddr(ADDR, W);
					I2CSendByte(actuadores);
					I2CSendStop();
					fillRect(&zone_3c, Green);
				}
			}
			if(zoneNewPressed(&zone_3d)) { // Generador de Vacío 3
				if(actuadores & GV3) { // ON->OFF
					actuadores &= ~GV3;
					I2CSendAddr(ADDR, W);
					I2CSendByte(actuadores);
					I2CSendStop();
					fillRect(&zone_3d, Black);
				}
				else {								// OFF->ON
					actuadores |= GV3;
					I2CSendAddr(ADDR, W);
					I2CSendByte(actuadores);
					I2CSendStop();
					fillRect(&zone_3d, Green);
				}
			}
			if(zoneNewPressed(&zone_3e)) { // Generador de Vacío 4
				if(actuadores & GV4) { // ON->OFF
					actuadores &= ~GV4;
					I2CSendAddr(ADDR, W);
					I2CSendByte(actuadores);
					I2CSendStop();
					fillRect(&zone_3e, Black);
				}
				else {								// OFF->ON
					actuadores |= GV4;
					I2CSendAddr(ADDR, W);
					I2CSendByte(actuadores);
					I2CSendStop();
					fillRect(&zone_3e, Green);
				}
			}
			if(zoneNewPressed(&zone_3f)) { // Generador de Vacío 5
				if(actuadores & GV5) { // ON->OFF
					actuadores &= ~GV5;
					I2CSendAddr(ADDR, W);
					I2CSendByte(actuadores);
					I2CSendStop();
					fillRect(&zone_3f, Black);
				}
				else {								// OFF->ON
					actuadores |= GV5;
					I2CSendAddr(ADDR, W);
					I2CSendByte(actuadores);
					I2CSendStop();
					fillRect(&zone_3f, Green);
				}
			}
			break;
		case PULSO:
			if(pulsos>=2) {
				// Tras 2 pulsos (un periodo) paramos los motores
				MOTOR_X_OFF;
				MOTOR_Z_OFF;
				state = MAIN_WAIT;
			}
		break;
		default:
			state = MAIN_WAIT;
			break;
	}
}

void TIMER1_IRQHandler(void)
{
	if((LPC_TIM1->IR & 2)>>1 == 1)
	{
		LPC_TIM1->IR |= 1<<1;					// Borra el flag MR1
		pulsos++;
	}
}

void TIMER0_IRQHandler(void)
{
	if((LPC_TIM0->IR & 2)>>1 == 1)
	{
		LPC_TIM0->IR |= 1<<1;					// Borra el flag MR1
		pulsos++;
	}
}
