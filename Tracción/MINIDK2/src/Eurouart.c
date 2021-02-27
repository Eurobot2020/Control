/*
    En esta biblioteca incluyo las funciones clásicas de la UART y las propias del protocolo de este año(Eurobot 2020).
    Las funciones de este año se encuentran al final del documento

    Autor: Javier Ortiz Perez-Jaraiz    (Trompiz, el que no gestiona)
    Año: 2020


*/

#include "Eurouart.h" //Enlazo con la biblioteca
#include <LPC17xx.h>
#include "variables.h"
//Configuraci�n de la Baudrate
static int uart0_set_baudrate(unsigned int baudrate) //Esta funci�n nos permite ajustar los registros de la MiniDK2 para ajustar al BAUDRATE con una gran precision. Funcion extraida de un ejemplo de la bb.
{
    int errorStatus = -1; //< Failure

    // UART clock (FCCO / PCLK_UART0)
    // unsigned int uClk = SystemCoreClock / 4;
    unsigned int uClk = SystemCoreClock / 4;
    unsigned int calcBaudrate = 0;
    unsigned int temp = 0;

    unsigned int mulFracDiv, dividerAddFracDiv;
    unsigned int divider = 0;
    unsigned int mulFracDivOptimal = 1;
    unsigned int dividerAddOptimal = 0;
    unsigned int dividerOptimal = 0;

    unsigned int relativeError = 0;
    unsigned int relativeOptimalError = 100000;

    uClk = uClk >> 4; /* div by 16 */

    /*
     *  The formula is :
     * BaudRate= uClk * (mulFracDiv/(mulFracDiv+dividerAddFracDiv) / (16 * DLL)
     *
     * The value of mulFracDiv and dividerAddFracDiv should comply to the following expressions:
     * 0 < mulFracDiv <= 15, 0 <= dividerAddFracDiv <= 15
     */
    for (mulFracDiv = 1; mulFracDiv <= 15; mulFracDiv++)
    {
        for (dividerAddFracDiv = 0; dividerAddFracDiv <= 15; dividerAddFracDiv++)
        {
            temp = (mulFracDiv * uClk) / (mulFracDiv + dividerAddFracDiv);

            divider = temp / baudrate;
            if ((temp % baudrate) > (baudrate / 2))
                divider++;

            if (divider > 2 && divider < 65536)
            {
                calcBaudrate = temp / divider;

                if (calcBaudrate <= baudrate)
                {
                    relativeError = baudrate - calcBaudrate;
                }
                else
                {
                    relativeError = calcBaudrate - baudrate;
                }

                if (relativeError < relativeOptimalError)
                {
                    mulFracDivOptimal = mulFracDiv;
                    dividerAddOptimal = dividerAddFracDiv;
                    dividerOptimal = divider;
                    relativeOptimalError = relativeError;
                    if (relativeError == 0)
                        break;
                }
            }
        }

        if (relativeError == 0)
            break;
    }

    if (relativeOptimalError < ((baudrate * UART_ACCEPTED_BAUDRATE_ERROR) / 100))
    {

        LPC_UART0->LCR |= DLAB_ENABLE; // importante poner a 1
        LPC_UART0->DLM = (unsigned char)((dividerOptimal >> 8) & 0xFF);
        LPC_UART0->DLL = (unsigned char)dividerOptimal;
        LPC_UART0->LCR &= ~DLAB_ENABLE; // importante poner a 0

        LPC_UART0->FDR = ((mulFracDivOptimal << 4) & 0xF0) | (dividerAddOptimal & 0x0F);

        errorStatus = 0; //< Success
    }

    return errorStatus;
}

void uart0_init(int baudrate)
{
    //Comunicaci�n por USB, asi que no hace falta ningun USB
    LPC_PINCON->PINSEL0 |= (1 << 4) | (1 << 6); // Change P0.2 and P0.3 mode to TXD0 and RXD0

    LPC_UART0->LCR &= ~STOP_1_BIT & ~PARITY_NONE; // Set 8N1 mode (8 bits/dato, sin pariad, y 1 bit de stop)
    LPC_UART0->LCR |= CHAR_8_BIT;

    uart0_set_baudrate(baudrate); // Set the baud rate

    LPC_UART0->IER = (1 << 1) | (1 << 0); // Interrupciones habilitadas de transmisi�n y recepci�n, no se activa ninguna de fallos.
    NVIC_EnableIRQ(UART0_IRQn);           // Enable the UART interrupt (for Cortex-CM3 NVIC)
    NVIC_SetPriority(UART0_IRQn, 1);      //Asigno prioridad
}

void UART0_IRQHandler(void)
{
    static int i = 0;

    switch (LPC_UART0->IIR & 0x0E)
    {

    case 0x04: /* Dato recibido */
        //transmitir_cadenaUART0("X");
        //*ptr_rx=LPC_UART0->RBR;
        buffer[i] = LPC_UART0->RBR; /* lee el dato recibido y lo almacena */

        if (buffer[i] < 31) // Caracter return --> Cadena completa

        {
            //transmitir_cadenaUART0("R");
            /*if (i != bits_esperados)
                error_longitud = 1; */
            i = 0;
            rx_completa = 1; /* rx completa */
        }
        else
            i++;
        break;

    case 0x02: /*Interrupciónn activada para enviar*/
        if (*ptr_tx != 0)
            LPC_UART0->THR = *ptr_tx++; /* carga un nuevo dato para ser transmitido */
        else
            tx_completa = 1;
        break;
    }
}
void transmitir_cadenaUART0(char *cadena)
{
    ptr_tx = cadena;
    tx_completa = 0;
    LPC_UART0->THR = *ptr_tx++; // IMPORTANTE: Introducir un car�cter al comienzo para iniciar TX o
} // activar flag interrupci�n por registro transmisor vacio

//Funciones nuevas:
void Traduccion_Variables(cinematica *variable)
{ //Se encarga de leer el mensaje recibido, actualizar las variables y levantar los flags.
    switch (T_INSTRUCCION)
    {
    case ('G'):
        Instruccion_Codigo = T_INSTRUCCION;
        Instruccion_Prioridad = BIT_PRIO-'0';
        grados_giro = (BIT_C_GRADOS - '0') * 100 + (BIT_D_GRADOS - '0') * 10 + (BIT_U_GRADOS - '0');
				variable->angulo=grados_giro;
        if (BIT_SIGNO_GRADOS == '+')
        {
            grados_giro = grados_giro;
        }
        else if (BIT_SIGNO_GRADOS == '-')
        {
            grados_giro = -1 * grados_giro;
        }
        else
        {
            transmitir_cadenaUART0("ES");
        }

        break;
    case ('D'):
        Instruccion_Codigo = T_INSTRUCCION;
        Instruccion_Prioridad = BIT_PRIO-'0';
        distancia = (BIT_M_DISTANCIA - '0') * 1000 + (BIT_C_DISTANCIA - '0') * 100 + (BIT_D_DISTANCIA - '0') * 10 + (BIT_U_DISTANCIA - '0');
				variable->distancia=distancia;
        velocidad_final = (BIT_M_V_FINAL - '0') * 1000 + (BIT_C_V_FINAL - '0') * 100 + (BIT_D_V_FINAL - '0') * 10 + (BIT_U_V_FINAL - '0');
        velocidad_maxima = (BIT_M_VMAX - '0') * 1000 + (BIT_C_VMAX - '0') * 100 + (BIT_D_VMAX - '0') * 10 + (BIT_U_VMAX - '0');
        if (BIT_SIGNO_GRADOS == '+')
            distancia = distancia;
        else if (BIT_SIGNO_GRADOS == '-')
            distancia = -1 * distancia;
        else
            transmitir_cadenaUART0("ES");
        break;
    case ('C'):
        Instruccion_Codigo = T_INSTRUCCION;
        Instruccion_Prioridad = BIT_PRIO-'0';
        distancia = (BIT_M_DISTANCIA_C - '0') * 1000 + (BIT_C_DISTANCIA_C - '0') * 100 + (BIT_D_DISTANCIA_C - '0') * 10 + (BIT_U_DISTANCIA_C - '0');
        velocidad_final = (BIT_M_V_FINAL_C - '0') * 1000 + (BIT_C_V_FINAL_C - '0') * 100 + (BIT_D_V_FINAL_C - '0') * 10 + (BIT_U_V_FINAL_C - '0');
        velocidad_maxima = (BIT_M_VMAX_C - '0') * 1000 + (BIT_C_VMAX_C - '0') * 100 + (BIT_D_VMAX_C - '0') * 10 + (BIT_U_VMAX_C - '0');
        radio = (BIT_M_RADIO_C - '0') * 1000 + (BIT_C_RADIO_C - '0') * 100 + (BIT_D_RADIO_C - '0') * 10 + (BIT_U_RADIO_C - '0');
        break;

        //Instruccion_Prioridad = BIT_PRIO - '0'; //URGENTE PARAR
    case ('F'):
        Instruccion_Codigo = T_INSTRUCCION;
        Instruccion_Prioridad = 1; 
				break;
		
		case ('E'):		//REQUEST DEL ESTADO POR NIVEL ALTO
				Request = 1;
				break;
		
    default:
        transmitir_cadenaUART0("EI");
    
    }
}
