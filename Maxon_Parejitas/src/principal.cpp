/**** Inclusión de bibliotecas ****/
#include <Arduino.h>
#include <stdio.h>
#include <string.h>
// #include "PinChangeInterrupt.h"

#include "Eurouart.h"
#include "funciones_pwm.h"
#include "funciones_desplazamiento.h"
#include "funciones_control.h"
#include "configuraciones.h"
#include "funciones_timer.h"
#include "variables.h"


#define FPCLK 25e6
#define error_final 			0.01
#define V_FRENO           0.8      //rad/s

/**** Variables de la comunicación por UART (by Lesmus Trompiz) ****/

String inputString = "";          // a String to hold incoming data
bool stringComplete = false;  		// whether the string is complete

#define BAUDRATE 115200
char buffer[70];  // Buffer de recepci�n de 70 caracteres
char *ptr_rx;     // puntero de recepci�n
char rx_completa; // Flag de recepci�n de cadena que se activa a "1" al recibir la tecla return CR(ASCII=13)
char *ptr_tx;     // puntero de transmisi�n
char tx_completa; // Flag de transmisi�n de cadena que se activa al transmitir el caracter null (fin de cadena)

//char estado = 'NULL';
double vel_real = 0;
int distancia = 0;
int velocidad_final = 0;
int velocidad_maxima = 0;
int radio = 0;
int grados_giro = 0;

char aux[100]={0};	//Buffer de transmisión de 100 caracteres


static char Inicio = 1;

char Request = 0;
/**** CINEMÁTICA ****/

cinematica lazo_abierto;							//Estructura de variables cinemáticas
param_mecanicos maxon;								//Estructura de parámetros mecánicos

bool vel_fin_cambiar;
bool sentido_dcha;
bool sentido_izq;
bool parado;
bool parado_seguridad;
/**** Estados de la máquina de estados ****/

#define 	ST_INICIAL			0
#define		ST_PARADO				1
#define		ST_RECTA				2
#define		ST_GIRO					3
#define		ST_CURVA				4
#define		ST_FRENO				5

/**** Prioridades de la comunicación ****/

#define 	NORMAL 					0					//No asignar a ninguna instrucción que nos haga movernos
#define		URGENTE					1					//Freno

/**** Caracterización del robot ****/

Caracterizacion Robot;									//Estructura status del robot
T_Counter cuadratura;

/**** Caracterización de los mensajes ****/

char Instruccion_Codigo = ST_INICIAL;					//Aqui se guardan las los comandos
char Instruccion_Prioridad = NORMAL;					//Aqui almacenamos la prioridad

/****  Variables maquina estados  ****/

int flag_safety_freno = 1;
int Flag_EstadoFinalizado = 1;                           //La inicializo a 1 para que actualice Estado desde ST_INICIAL
int Siguiente_Estado = ST_INICIAL; 	
int Estado = ST_INICIAL;

long contador = 0;



/**** Funciones odometría ****/

void reset_odometria(void){
  cuadratura.contador_derecho_total=0;
  cuadratura.contador_izquierdo_total=0;
  cuadratura.contador_derecho_total_sumado = 0;
  cuadratura.contador_izquierdo_total_sumado = 0;
}

void reset_pose(void) {
  Robot.Orientacion = 0.0;
  Robot.Pos.X = 0.0;
  Robot.Pos.Y = 0.0;
}

void transmitir_estadoO(){
  // ardprintf("S X%d Y%d A%d\n",(int)Robot.Pos.X,(int)Robot.Pos.Y,(int)Robot.Orientacion);
  // Serial.println("ODOMETRIA");
  // Serial.print(Flag_EstadoFinalizado);
  // if(Flag_EstadoFinalizado)
  //   Serial.print("S");
  // else  
  //   Serial.print("O");
  Serial.print("O");
  Serial.print("X");
  Serial.print(float(Robot.Pos.X));
  Serial.print("Y");
  Serial.print(float(Robot.Pos.Y));
  Serial.print("A");
  Serial.print(float(Robot.Orientacion));
  Serial.print("V");
  Serial.println(int(Robot.VelActual));
}


void transmitir_estadoS(){
  // ardprintf("S X%d Y%d A%d\n",(int)Robot.Pos.X,(int)Robot.Pos.Y,(int)Robot.Orientacion);
  // Serial.println("ODOMETRIA");
  // Serial.print(Flag_EstadoFinalizado);
  // if(Flag_EstadoFinalizado)
  //   Serial.print("S");
  // else  
  //   Serial.print("O");
  Serial.print("S");
  Serial.print("X");
  Serial.print(float(Robot.Pos.X));
  Serial.print("Y");
  Serial.print(float(Robot.Pos.Y));
  Serial.print("A");
  Serial.print(float(Robot.Orientacion));
  Serial.print("V");
  Serial.println(int(Robot.VelActual));
  
}
int CMD_Inicial(void){
//En el bucle general se llama a esta función que inicializa la estructura Robot y espera instrucciones para arrancar hacia otro estado



  if(Inicio){

    // Inicio=0;
    //Flag_EstadoFinalizado=0			//En ST_INICIAL no hace falta ya que está deseoso de órdenes
    parado = true;
    reset_odometria();
    reset_pose();
    Robot.VelActual = 0.0;

  }

  //ESTE ESTADO NO HACE NADA APARTE DE INICIALIZAR

  //Un timer por debajo se encarga de transmitir la estructura a los de arriba cada 500 ms
  //cuando terminamos un comando tb se transmite la estructura del robot, esta vez de manera asíncrona
  
  return 0;
}

int CMD_Parado(void){
//Función de seguridad en el que llevamos todas las velocidades a 0 y miramos por la siguiente instrucción
//Seguramente estamos aquí por un mensaje de FRENO

static bool flag_timer = 1;
static bool envio = 1;
  if(Inicio){

    contador = 0;
    Inicio=0;
    Flag_EstadoFinalizado=0;				//En ST_PARADO SÍ hay que hacerlo inicialmente ya que solemos llegar por ENEMIGO
    // Serial.println("EP");
    flag_safety_freno = 0;
    apaga_motores();                //Deshabilita por aqui para que no se pase la distancia, el parón es instantáneo aunque depende de la inercia
                                    //Es mejor que se pruebe para cada robot
                                    //A mayor rueda menor V a poner en la frenada y viceversas
    // Serial.println("DISABLED");
  }


  if(contador > 20 && flag_timer){ //Obsoleto (?)

    flag_timer = 0;
    // apaga_motores();
    // Serial.println("DISABLED");
  }
  
  if (parado && envio)
  { 
    envio = 0;
    transmitir_estadoS();
    // Serial.println("DRACUPARADO");
    // Serial.println(cuadratura.contador_derecho_total);
    // Serial.println(cuadratura.contador_izquierdo_total);
    // Serial.println(cuadratura.contador_derecho_total_sumado);
    // Serial.println(cuadratura.contador_izquierdo_total_sumado);
    Robot.VelActual = 0.0;
    reset_pose();
    reset_odometria();
  }
  
  if(Instruccion_Codigo != ST_PARADO && !envio ){ // || Instruccion_Codigo != 'F'   && Instruccion_Codigo != 'F'

    Flag_EstadoFinalizado = 1;
    Inicio = 1;
    flag_timer = 1;
    // Serial.println("SP"); 
    flag_safety_freno = 1;
    envio = 1;
  }
  return 0;
}

int CMD_Recta(void){
//Avanzamos una distancia en línea recta a velocidad máxima definida


static bool Flag_Frenada = 1;
static bool flag_timer = 1; 
  if(Inicio){

    Inicio = 0;
    contador = 0;
    Flag_EstadoFinalizado = 0;
    flag_timer = 1;
    // Serial.println("E"); 

  }

  if( flag_timer && contador > 15 ){
    
    flag_timer = 0;
    transmitir_estadoO();
    reset_pose();
    reset_odometria();
    
    calcula_parametros_recta(&lazo_abierto,&maxon,&cuadratura);
    motores(&lazo_abierto,&maxon);
    // Serial.println("T");
    // Serial.println(lazo_abierto.distancia_total_rad);
    // Serial.println(lazo_abierto.distancia_acel_vel_cte);
  }

  //Evalúo el error que tengo en cada ciclo hasta que me toca frenar
  if ( (lazo_abierto.error_posicion_actual_derecha < lazo_abierto.ajustar_distancia || 
  lazo_abierto.error_posicion_actual_izquierda < lazo_abierto.ajustar_distancia) && Flag_Frenada && !flag_timer){
    
    // velocidad_derecha(lazo_abierto.velocidad_inicial,&maxon);
    // velocidad_izquierda(lazo_abierto.velocidad_inicial,&maxon);
    velocidad_derecha(V_FRENO,&maxon);
    velocidad_izquierda(V_FRENO,&maxon);
    Flag_Frenada = 0;
    // Serial.println("F");
    transmitir_estadoO();
  }
  
  //Evalúo el error hasta que llega a la posición exacta que me piden alcanzar
  else if((lazo_abierto.error_posicion_actual_derecha_total < error_final || 
  lazo_abierto.error_posicion_actual_izquierda_total < error_final) && !Flag_Frenada){
    //CUANDO LLEGA A LA POSICION FINAL SALE DEL ESTADO Y CARGA EL SIGUIENTE
    //en caso de que tenga error extraño puede ser por frenar y no esperar para cargar la siguiente instruccion


    velocidad_derecha(lazo_abierto.velocidad_inicial,&maxon);
    velocidad_izquierda(lazo_abierto.velocidad_inicial,&maxon);
    Flag_EstadoFinalizado = 1;                            //LEVANTO FLAG LLEGADA PARA CARGAR EL SIGUIENTE ESTADO 
    Inicio = 1;
    Flag_Frenada = 1;
    flag_timer = 1;

  }
  return 0;
}

int CMD_Giro(void){
//Giramos sobre el eje del robot un ángulo definido a velocidad máxima definida

static bool Flag_Frenada = 1;
static bool flag_timer = 1;

  if(Inicio){

    Inicio = 0;
    contador = 0;
    Flag_EstadoFinalizado = 0;
    flag_timer = 1;
    // Serial.println("Entro en CMD_Giro"); 

  }


  if( flag_timer && contador >15 ){

    flag_timer = 0;
    transmitir_estadoO();
    reset_pose();
    reset_odometria();

    calcula_parametros_giro(&lazo_abierto,&maxon,&cuadratura);
    motores(&lazo_abierto,&maxon);
    // Serial.println("T");

  }
  
  //Evalúo el error que tengo en cada ciclo hasta que me toca frenar
  if ( (lazo_abierto.error_posicion_actual_derecha < lazo_abierto.ajustar_distancia || 
  lazo_abierto.error_posicion_actual_izquierda < lazo_abierto.ajustar_distancia) && Flag_Frenada && !flag_timer){
  
    velocidad_derecha(lazo_abierto.velocidad_inicial,&maxon);
    velocidad_izquierda(lazo_abierto.velocidad_inicial,&maxon);
    Flag_Frenada = 0; 
    // Serial.println("F");
    transmitir_estadoO();
  }

  //Evalúo el error hasta que llega a la posición exacta que me piden alcanzar
  else if((lazo_abierto.error_posicion_actual_derecha_total < error_final || 
  lazo_abierto.error_posicion_actual_izquierda_total < error_final) && !Flag_Frenada){
    //CUANDO LLEGA A LA POSICION FINAL SALE DEL ESTADO Y CARGA EL SIGUIENTE
    //en caso de que tenga error extraño puede ser por frenar y no esperar para cargar la siguiente instruccion

        
        Flag_EstadoFinalizado = 1;                            //LEVANTO FLAG LLEGADA PARA CARGAR EL SIGUIENTE ESTADO 
        Inicio = 1;
        Flag_Frenada = 1;
        flag_timer = 1;

  }
  
return 0;
}

int CMD_Curva(void){
//Tomamos una curva de radio determinado y angulo de arco determinado a velocidad máxima definida
  // Serial.println("CURVA");
  return 0;
}

int CMD_Freno(void){
//FRENA POR TU VIDA



  if (!Inicio)
  {

    Inicio = 1;
    
    calcula_parametros_freno_emergencia(&lazo_abierto,&maxon);
    motores(&lazo_abierto,&maxon);
    Flag_EstadoFinalizado = 0;
    // Serial.println("Entro en CMD_Freno");
  }
  
  
  //Evalúo el error que tengo en cada ciclo hasta que debería estar parado
  // if(lazo_abierto.error_posicion_actual_derecha_total < error_final || lazo_abierto.error_posicion_actual_izquierda_total < error_final){
  if(parado_seguridad){
      
      Flag_EstadoFinalizado = 1;                            //LEVANTO FLAG LLEGADA PARA CARGAR EL SIGUIENTE ESTADO 
      Inicio = 1;
      Instruccion_Codigo = ST_PARADO;			//Al no estar entre las opciones, se irá al default que es ST_PARADO
      
      // transmitir_estado();				//En este estado incluye la que se movió con el estado anterior, ya que no hubo flag_final al ser URGENTE
      // reset_pose();
      // reset_odometria();
      
  }
  
  
  return 0;
}
  
  

int Maquina_Estados(void){
  
    // Selección del próximo estado
    switch(Instruccion_Codigo){
      case ST_INICIAL:
        Siguiente_Estado = ST_INICIAL;
        break;
      case 'D':
        Siguiente_Estado = ST_RECTA;				
        break;
      case 'C':
        Siguiente_Estado = ST_CURVA;				
        break;
      case 'G':
        Siguiente_Estado = ST_GIRO;			
        break;
      case 'F':
        Siguiente_Estado = ST_FRENO;				
        break;
      default:										
        Siguiente_Estado = ST_PARADO;			
        break;
    }
    
    // Si el estado ha finalizado o hay un mensaje de prioridad urgente -> cambio de estado
    if(Instruccion_Prioridad || Flag_EstadoFinalizado ){ //|| flag_safety_freno
      Estado = Siguiente_Estado;
      // Serial.println(Estado);
      //Limpia el mensaje para que en caso de no recibir nada se pare
      Instruccion_Codigo = ST_PARADO;
      Instruccion_Prioridad = NORMAL;
    }
    
    if(stringComplete)
    {
    Traduccion_Variables(&lazo_abierto);
    stringComplete = false;
    // Serial.println("Traducido");
    }
    
    
    if(Request)
    {
      transmitir_estadoO();
      Request = 0;
    }
    return 0;
}



/**** Funci�n main ****/

void setup(){
Serial.begin(BAUDRATE);
// Serial.println("Debugging...");
//Configuramos TODO lo configurable

// init_pwm();

init_odom();
config_TIMER0();
config_TIMER1();
configuracion_parametros_mecanicos(&maxon,&lazo_abierto);
enciende_motores(); 
// sentido_motores(0,0);

}

void loop(){

/*   if(contador == 50){
		Serial.println("Blinkeo");
		contador = 0;
	} */
  // Ejecutamos la máquina de estados
  Maquina_Estados();
  
  // Super bucle para cambiar de estado
  switch(Estado){	
  
      /* En el estado inicial, esperamos la llegada de la primera instrucción	 
      basado en la estrategia. */
      case ST_INICIAL:
                                                  // nuevo_estado = estado_inicial(Estado)
                                                  //Hago una primera lectura de todos los sensores
                                                  //Espero por la UART la primera instrucción
          CMD_Inicial();
          // Serial.println("CMD_Inicial");
          break;
      
      /* Estado PARADO: nos quedamos a la espera de la siguiente instrucci�n.
      Se puede llegar aqu� por: ausencia de tareas, enemigo cercano. */
      case ST_PARADO:
                                                  //PAREN LAS ROTATIVAS
                                                  //Hago lo mismo que en INICIAL 
          CMD_Parado();    
          // Serial.println("CMD_Parado");        
          break;
      
      /* Estado RECTA: instrucci�n de movimiento en l�nea recta. Se evalua:
      la vel. actual, la vel. m�xima, la vel. final y la distancia */
      case ST_RECTA:
          CMD_Recta();
          // Serial.println("CMD_Recta"); 
                                                  //SOLO EN EL PRIMER CICLO DEL ESTADO
                                                      //Cargamos los valores de distancia y velocidades
                                                      //Comenzamos la medida de odometría
                                                      //Habilitamos motores
                                                  //TODOS LOS CICLOS
                                                      //Controlamos lo que falta para llegar
          break;
      
      /* Estado GIRO: instrucci�n de giro sobre el propio eje. Se evalua:
      la vel. actual, la vel. final y �ngulo de giro */
      case ST_GIRO:
          CMD_Giro();
          // Serial.println("CMD_Giro"); 
          break;
      
      /* Estado CURVA: instrucci�n de movimiento tomando una curva. Se evalua:
      �ngulo, radio de curvatura, vel. actual y vel. final */
      case ST_CURVA:
          CMD_Curva();
          // Serial.println("CMD_Curva"); 
          break;
      
      /* Estado FRENO: instrucci�n de frenada de emergencia. No se evalua nada.
      Caso particular de RECTA */
      case ST_FRENO:
          CMD_Freno();
          // Serial.println("CMD_Freno"); 
          break;
      
      default:
                                                  //PAREN LAS ROTATIVAS
                                                  //Hago lo mismo que en INICIAL 
          CMD_Parado();
          // Serial.println("CMD_Parado_default"); 
          Instruccion_Codigo= ST_PARADO;
          break;
        }
}
 
