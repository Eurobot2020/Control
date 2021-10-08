#ifndef UART_H_
#define UART_H_

#include <Arduino.h>
#include "variables.h"

// Accepted Error baud rate value (in percent unit)

//Parametros inamovibles comunicación:
#define UART_ACCEPTED_BAUDRATE_ERROR 3
#define CHAR_8_BIT (3 << 0)
#define STOP_1_BIT (0 << 2)
#define PARITY_NONE (0 << 3)
#define DLAB_ENABLE (1 << 7)
#define FIFO_ENABLE (1 << 0)
#define RBR_IRQ_ENABLE (1 << 0)
#define THRE_IRQ_ENABLE (1 << 1)
#define UART_LSR_THRE (1 << 5)
#define RDA_INTERRUPT (2 << 1)
#define CTI_INTERRUPT (6 << 1)

// ARDUCOSAS
#define ARDBUFFER 32
//Parametros de posición:
//Generales
#define BIT_PRIO inputString.charAt(0)
#define T_INSTRUCCION inputString.charAt(1) //Primer char, es el que identifica la instrucción
//Giro
#define BIT_SIGNO_GRADOS inputString.charAt(2)
#define BIT_C_GRADOS inputString.charAt(3)
#define BIT_D_GRADOS inputString.charAt(4)
#define BIT_U_GRADOS inputString.charAt(5)
//Desplazamiento
#define BIT_SIGNO_D inputString.charAt(2)
#define BIT_M_DISTANCIA inputString.charAt(3)
#define BIT_C_DISTANCIA inputString.charAt(4)
#define BIT_D_DISTANCIA inputString.charAt(5)
#define BIT_U_DISTANCIA inputString.charAt(6)
#define BIT_M_V_FINAL inputString.charAt(7)
#define BIT_C_V_FINAL inputString.charAt(8)
#define BIT_D_V_FINAL inputString.charAt(9)
#define BIT_U_V_FINAL inputString.charAt(10)
#define BIT_M_VMAX inputString.charAt(11)
#define BIT_C_VMAX inputString.charAt(13)
#define BIT_D_VMAX inputString.charAt(14)
#define BIT_U_VMAX inputString.charAt(15)
//Curva
#define BIT_M_DISTANCIA_C inputString.charAt(2)
#define BIT_C_DISTANCIA_C inputString.charAt(3)
#define BIT_D_DISTANCIA_C inputString.charAt(4)
#define BIT_U_DISTANCIA_C inputString.charAt(5)
#define BIT_M_V_FINAL_C inputString.charAt(6)
#define BIT_C_V_FINAL_C inputString.charAt(7)
#define BIT_D_V_FINAL_C inputString.charAt(8)
#define BIT_U_V_FINAL_C inputString.charAt(9)
#define BIT_M_VMAX_C inputString.charAt(10)
#define BIT_C_VMAX_C inputString.charAt(11)
#define BIT_D_VMAX_C inputString.charAt(12)
#define BIT_U_VMAX_C inputString.charAt(13)
#define BIT_M_RADIO_C inputString.charAt(14)
#define BIT_C_RADIO_C inputString.charAt(15)
#define BIT_D_RADIO_C inputString.charAt(16)
#define BIT_U_RADIO_C inputString.charAt(17)

extern String inputString;         // a String to hold incoming data
extern bool stringComplete;  // whether the string is complete



extern char *ptr_rx;     // Puntero de recepción
extern char rx_completa; // Flag de recepción de cadena que se activa a "1" al recibir la tecla return CR(ASCII=13)
extern char *ptr_tx;     // Puntero de transmisión
extern char tx_completa; // Flag de transmision completa
extern char buffer[70];  // Buffer de recepción de caracteres

extern char estado;
extern int distancia;
extern int velocidad_final;
extern int velocidad_maxima;
extern int radio;
extern int grados_giro;
extern char Instruccion_Codigo;
extern char Instruccion_Prioridad;
extern void(* resetFunc) (void);
 
extern void Traduccion_Variables(cinematica *variable);
extern void serialEvent(void);
extern int  ardprintf(char *str, ...);


#endif /* UART_H_ */
