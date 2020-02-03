/*****************************************
************** constants.h  **************

Definiciones de todas las constantes del programa:
	- Pines utilizados
	- Características para los motores
	- ON/OFF electrovalvulas y generador de vacío
	- Estados de la máquina de estados
*****************************************/

// Pines utilizados

#define PIN_MOTOR_X_PWM		25	// P1.25 en modo MAT1.1
#define PIN_MOTOR_X_STEP	23	// P1.23
#define PIN_MOTOR_Z_PWM		26	// P3.26 en modo MAT0.1
#define PIN_MOTOR_Z_STEP	28	// P0.28
#define SDA								0		// P0.0 en modo SDA1 
#define SCL								1		// P0.1 en modo SCL1
#define PIN_FIN_CARRERA		5 	// P.-P. 

// Características para los motores

#define L_EJE_X 10	// Recorrido del eje x en mm
#define L_EJE_Z 150	// Recorrido del eje z en mm
#define PASOS_POR_VUELTA 200 // Pasos por vuelta de nuestro motor
#define T_PWM	(PASOS_POR_VUELTA/10000)	// 
#define PASOS_PERIODO (T_PWM*100e6*0.001) // Calculo del periodo de actuación sobre el pin STEP.
#define ARRIBA (LPC_GPIO0->FIOSET |= (1<<PIN_MOTOR_Z_STEP)) // Comprobar lado una vez montado
#define ABAJO (LPC_GPIO0->FIOCLR |= (1<<PIN_MOTOR_Z_STEP))
#define DERECHA (LPC_GPIO1->FIOSET |= (1<<PIN_MOTOR_X_STEP))
#define IZQUIERDA (LPC_GPIO1->FIOCLR |= (1<<PIN_MOTOR_X_STEP))
#define MOTOR_X_ON (LPC_TIM1->TCR |= 2, LPC_TIM1->TCR &= ~2,	LPC_TIM1->TCR |= 1)
#define MOTOR_X_OFF (LPC_TIM1->TCR &= ~1)
#define MOTOR_Z_ON (LPC_TIM0->TCR |= 2, LPC_TIM0->TCR &= ~2,	LPC_TIM0->TCR |= 1)
#define MOTOR_Z_OFF (LPC_TIM0->TCR &= ~1)

// ON/OFF electrovalvulas y generador de vacío

#define ADDR 0x20
#define W 0
#define R 1
#define GV1 0x10
#define GV2 0x08
#define GV3 0x04
#define GV4 0x02
#define GV5	0x01

// Estados de la máquina de estados
#define MAIN 			0
#define MAIN_WAIT 1
#define PULSO			2


