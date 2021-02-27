#ifndef UART_H_
#define UART_H_

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
//Parametros de posición:
//Generales
#define BIT_PRIO buffer[0]
#define T_INSTRUCCION buffer[1] //Primer char, es el que identifica la instrucción
//Giro
#define BIT_SIGNO_GRADOS buffer[2]
#define BIT_C_GRADOS buffer[3]
#define BIT_D_GRADOS buffer[4]
#define BIT_U_GRADOS buffer[5]
//Desplazamiento
#define BIT_SIGNO_D buffer[2]
#define BIT_M_DISTANCIA buffer[3]
#define BIT_C_DISTANCIA buffer[4]
#define BIT_D_DISTANCIA buffer[5]
#define BIT_U_DISTANCIA buffer[6]
#define BIT_M_V_FINAL buffer[7]
#define BIT_C_V_FINAL buffer[8]
#define BIT_D_V_FINAL buffer[9]
#define BIT_U_V_FINAL buffer[10]
#define BIT_M_VMAX buffer[11]
#define BIT_C_VMAX buffer[12]
#define BIT_D_VMAX buffer[13]
#define BIT_U_VMAX buffer[14]
//Curva
#define BIT_M_DISTANCIA_C buffer[2]
#define BIT_C_DISTANCIA_C buffer[3]
#define BIT_D_DISTANCIA_C buffer[4]
#define BIT_U_DISTANCIA_C buffer[5]
#define BIT_M_V_FINAL_C buffer[6]
#define BIT_C_V_FINAL_C buffer[7]
#define BIT_D_V_FINAL_C buffer[8]
#define BIT_U_V_FINAL_C buffer[9]
#define BIT_M_VMAX_C buffer[10]
#define BIT_C_VMAX_C buffer[11]
#define BIT_D_VMAX_C buffer[12]
#define BIT_U_VMAX_C buffer[13]
#define BIT_M_RADIO_C buffer[14]
#define BIT_C_RADIO_C buffer[15]
#define BIT_D_RADIO_C buffer[16]
#define BIT_U_RADIO_C buffer[17]

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

extern void uart0_init(int baudrate);
extern void transmitir_cadenaUART0(char *cadena);
extern void Traduccion_Variables(cinematica *variable);


#endif /* UART_H_ */
