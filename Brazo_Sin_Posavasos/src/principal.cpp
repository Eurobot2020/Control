#include <Arduino.h>

//Shield Motores Paso a Paso
// 60mm laterales
// 70mm verticales
#define EN          8       // stepper motor enable, low level effective
#define X_DIR       2       //X axis, stepper motor direction control 
#define Y_DIR       3       //y axis, stepper motor direction control
#define Z_DIR       4       //z axis, stepper motor direction control
#define X_STP       5       //x axis, stepper motor control
#define Y_STP       6       //y axis, stepper motor control
#define Z_STP       7       //z axis, stepper motor control
#define PASO        8       //Paso del tornillo de 8mm/rev
#define PULSOS_REV  200     //Número de pasos del motor NEMA por rev (pasos/rev)
// Ventosas
#define IZQ 11              // D11 = Z+ y Z-  ventosa_izq (NARANJA (?))
#define MEDIZQ 10           // D10 = Y+ e Y- ventosa_medizq
#define MEDIO 9             // D9 = X+ y X- ventosa_medio
#define MEDDCHA 13          // D13 = ventosa_meddcha
#define DCHA 12             // D12 = ventosa_dcha (MORADO OSCURO (?))
// //Neumática
// #define VACIO 1
// #define REPOSO 0
// Finales de carrera - ENDSTOP         5V Sin pulsar & 0V Pulsado
#define UP_V      A0       //  
#define DOWN_V    A1       //Hold         ROJO SIN CONECTAR DA IGUAL EL SENTIDO(Para los de PCB)
#define UP_H      A2       //Resume       NO y Común, da igual el sentido
#define DOWN_H    A3       //CoolEN
#define ASCENDENTE 1
#define DESCENDENTE 0
// Estados
#define ST_INICIAL 0
#define ST_PARADO 1
#define VERTICAL 2
#define HORIZONTAL 3
//Comunicación Serie UART
#define NORMAL 0
#define URGENTE 1
#define ARDBUFFER 16

//Variables de comunicación
char Instruccion_Codigo = ST_INICIAL;         //Aqui se guardan  los comandos
char Instruccion_Prioridad = NORMAL;          //Aqui almacenamos la prioridad
char signo = 0;                               
int decimetros = 0;                           
int centimetros = 0;                          //Variables de distancia
int milimetros = 0;
int distancia = 0;
bool ventosa_izq = 0, ventosa_medizq = 0, ventosa_medio = 0, ventosa_meddcha = 0, ventosa_dcha = 0;
bool Request = 0;

//Variables maquina de estado
bool Flag_EstadoFinalizado = 1;
int Estado = ST_INICIAL;
int Siguiente_Estado = ST_PARADO;
bool Inicio = 1;
bool Flag_Horizontal = 0;
bool Flag_Vertical = 0;

// bool flag_UP_V
int flag_V_UP = !digitalRead(UP_V);
int flag_V_DOWN = !digitalRead(DOWN_V);
int flag_H_UP = !digitalRead(UP_H);
int flag_H_DOWN = !digitalRead(DOWN_H);
bool sentido = DESCENDENTE;

//Variables de timer y LED
int timer = 0;
int pwm = 0;
int cuentas_x = 0;
int cuentas_y = 0;
bool state=0;

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

/********************************COMANDOS******************************/
void(* resetFunc) (void) = 0;

void transmitir_estado(void){
  if(Flag_EstadoFinalizado)
    Serial.println('L');
}

int ardprintf(char *str, ...)
{
  int i, count=0, j=0, flag=0;
  char temp[ARDBUFFER+1];
  for(i=0; str[i]!='\0';i++)  if(str[i]=='%')  count++;

  va_list argv;
  va_start(argv, count);
  for(i=0,j=0; str[i]!='\0';i++)
  {
    if(str[i]=='%')
    {
      temp[j] = '\0';
      Serial.print(temp);
      j=0;
      temp[0] = '\0';

      switch(str[++i])
      {
        case 'd': Serial.print(va_arg(argv, int));
                  break;
        case 'l': Serial.print(va_arg(argv, long));
                  break;
        case 'f': Serial.print(va_arg(argv, double));
                  break;
        case 'c': Serial.print((char)va_arg(argv, int));
                  break;
        case 's': Serial.print(va_arg(argv, char *));
                  break;
        default:  ;
      };
    }
    else 
    {
      temp[j] = str[i];
      j = (j+1)%ARDBUFFER;
      if(j==0) 
      {
        temp[ARDBUFFER] = '\0';
        Serial.print(temp);
        temp[0]='\0';
      }
    }
  };
  Serial.println();
  return count + 1;
}

void Leer_mensaje(void){
  if (stringComplete) {
    
    // Serial.print(inputString);
    Instruccion_Codigo = inputString.charAt(0);
    // Serial.println(Instruccion_Codigo);

    if(Instruccion_Codigo == 'H' || Instruccion_Codigo == 'V'){
      signo = inputString.charAt(1);
      decimetros = inputString.charAt(2) - '0';
      centimetros = inputString.charAt(3) - '0';
      milimetros = inputString.charAt(4) - '0';
      distancia = decimetros*100 + centimetros*10 + milimetros;
      if(distancia > 100){
        distancia = 100;
      }
      // ardprintf("%c%d",signo, distancia);
    }
    else if (Instruccion_Codigo == 'N') {
      ventosa_izq = inputString.charAt(1) - '0';
      ventosa_medizq = inputString.charAt(2) - '0';
      ventosa_medio = inputString.charAt(3) - '0';
      ventosa_meddcha = inputString.charAt(4) - '0';
      ventosa_dcha = inputString.charAt(5) - '0';
      // ardprintf("%d%d%d%d%d",ventosa_izq,ventosa_medizq,ventosa_medio,
      // ventosa_meddcha,ventosa_dcha);
      
    }

    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void CMD_Inicial(void){       //COMANDO INICIAL: este se ejecutará una única vez
  if(Inicio){
    // Serial.println("Hello there");
    // Serial.println("ST_INICIAL"); 
    Inicio = 0;
    Flag_Horizontal = 0;
    Flag_Vertical = 0;
    //Flag_EstadoFinalizado = 0;          //NO LO BORRAMOS PORQUE ES SIEMPRE TAMOS READY
  }
  
  if(Instruccion_Codigo != ST_INICIAL){

		Flag_EstadoFinalizado = 1;
		Inicio = 1;

	}  
}

void CMD_Parado(void){        //COMANDO PARADO: se asegurará que los motores estés parados
  if(Inicio){
    // Serial.println("ST_PARADO");
    transmitir_estado();
    Inicio = 0;
    //Flag_EstadoFinalizado = 0;          //NO LO BORRAMOS PORQUE SIEMPRE TAMOS READY
    Flag_Horizontal = 0;
    Flag_Vertical = 0;
  }

  if(Instruccion_Codigo != ST_PARADO && Instruccion_Codigo != 'N'){

		Flag_EstadoFinalizado = 1;
		Inicio = 1;

	}  
}

void CMD_Vertical(void){      //COMANDO VERTICAL: hace que el brazo se desplace verticalmente hacia arriba y abajo
  if(Inicio){
    // Serial.println("VERTICAL");
    digitalWrite(EN, LOW);
    Inicio = 0;
    Flag_EstadoFinalizado = 0;
    cuentas_y = 0;
    Flag_Vertical = 1;
    Flag_Horizontal = 0;      //De momento que sea puro 1 eje cada vez
    if(signo == '+'){
        
      PORTD |= 1 << Y_DIR;
      sentido = ASCENDENTE;
    }
    else{
      PORTD &= ~(1 << Y_DIR);
      sentido = DESCENDENTE;
    }
  }

  // if(cuentas_y/2 >= distancia*PULSOS_REV/PASO || !digitalRead(UP_V) || !digitalRead(DOWN_V)){
  if(cuentas_y/2 >= distancia*PULSOS_REV/PASO || (!digitalRead(UP_V) && sentido) || (!digitalRead(DOWN_V) && !sentido)){
    Flag_EstadoFinalizado = 1;
    Inicio = 1;
    cuentas_y = 0;
    Flag_Vertical = 0;
    digitalWrite(EN, HIGH);
    // transmitir_estado();
  }
}

void CMD_Horizontal(void){    //COMANDO HORIZONTA: hace que el brazo se desplace horizontalmente hacia derecha e izquierda
  if(Inicio){
    // Serial.println("HORIZONTAL");
    digitalWrite(EN, LOW);
    Inicio = 0;
    Flag_EstadoFinalizado = 0;
    cuentas_x = 0;
    Flag_Horizontal = 1;
    Flag_Vertical = 0;            //De momento que sea puro 1 eje cada vez
    if(signo == '+'){
      PORTD |= 1 << X_DIR;
      sentido = ASCENDENTE;
    }
    else{
      PORTD &= ~(1 << X_DIR);  
      sentido = DESCENDENTE;
    }
  }

  // if(cuentas_x/2 >= distancia*PULSOS_REV/PASO || !digitalRead(UP_H) || !digitalRead(DOWN_H)){
  if(cuentas_x/2 >= distancia*PULSOS_REV/PASO || (!digitalRead(UP_H) && sentido)  || (!digitalRead(DOWN_H) && !sentido)){
    Flag_EstadoFinalizado = 1;
    Inicio = 1;
    Flag_Horizontal = 0;
    cuentas_x = 0;
    digitalWrite(EN, HIGH);
    // transmitir_estado();
  }
}

void actualiza_ventosas(bool vent_izq, bool vent_medizq, bool vent_medio, bool vent_meddcha, bool vent_dcha){
  digitalWrite(IZQ,vent_izq);
  digitalWrite(MEDIZQ,vent_medizq);
  digitalWrite(MEDIO,vent_medio);
  digitalWrite(MEDDCHA,vent_meddcha);
  digitalWrite(DCHA,vent_dcha);
}

void Maquina_Estados(void){
  switch (Instruccion_Codigo){
    case ST_INICIAL:
    Siguiente_Estado = ST_INICIAL;
    break;
    case 'V':
    Siguiente_Estado = VERTICAL;
    break;
    case 'H':
    Siguiente_Estado = HORIZONTAL;
    break;
    default:
    Siguiente_Estado = ST_PARADO;
    break;
  }

  if(Instruccion_Codigo == 'N'){       //Comprobamos si son las ventosas aparte de los estados
    actualiza_ventosas(ventosa_izq,ventosa_medizq,ventosa_medio,ventosa_meddcha,ventosa_dcha);
    transmitir_estado(); 
    // Serial.println("Ventosas funcionando"); 
    //Limpia mensaje para que en caso de no recibir nada vaya a ST_PARADO
    Instruccion_Codigo = ST_PARADO;
    Instruccion_Prioridad = NORMAL;
  }

  if (Instruccion_Codigo == 'R')
  {
    actualiza_ventosas(0,0,0,0,0);
    resetFunc();
  }
  

  if(Instruccion_Prioridad || Flag_EstadoFinalizado)
  {
    /*//Prints de terminal para transiciones de estado en EARLY DEBUG ==> COMPRIME ESTO
    //Dice el estado al que entra
     if(Estado != Siguiente_Estado)
      switch (Siguiente_Estado){
        case ST_INICIAL:
        Serial.println("ST_INICIAL");
        break;
        case VERTICAL:
        Serial.println("VERTICAL");
        break;
        case HORIZONTAL:
        Serial.println("HORIZONTAL");
        break;
        case ST_PARADO:
        Serial.println("ST_PARADO");
        break;
      } */
    //Actualiza el estado
    Estado = Siguiente_Estado;
    //Limpia mensaje para que en caso de no recibir nada vaya a ST_PARADO
    Instruccion_Codigo = ST_PARADO;
    Instruccion_Prioridad = NORMAL;
    
  }
 
  Leer_mensaje();
  
  if(Request){              //Actualmente no esperamos ninguna request pero por si acaso
    transmitir_estado();
    Request = 0;
  }
}

void config_Timer0(void){
  TCCR0A=(1<<WGM01);    //Set the CTC mode   
  OCR0A=0xF9; //Value for ORC0A for 0.125 ms 
  TCCR0B|=(1<<CS01);    //Set the prescale 1/8 clock
  TCCR0B&=~(1<<CS00);
  
  TIMSK0|=(1<<OCIE0A);   //Set the interrupt request
  sei(); //Enable interrupt
  
  
}

ISR(TIMER0_COMPA_vect){    //This is the interrupt request
  timer++;
  pwm++;

  // flag_V_UP = !digitalRead(UP_V);
  // flag_V_DOWN = !digitalRead(DOWN_V);
  // flag_H_UP = !digitalRead(UP_H);
  // flag_H_DOWN = !digitalRead(DOWN_H);

  if(timer>=8000){  //Medimos 1 s
    state=!state;
    timer=0;
    // PORTD ^= 1 << X_DIR; // sets digital pin HIGH/LOW
    // PORTB ^= bit (5); // Toggle the LED
    // PINB = bit (5); // Poner un 1 en el bit de PINn provoca un toggle, no cambia el nivel permanentemente como hace PORTn
  }

  if(pwm >= 8){               // mayor número = menor velocidad (NÚMERO ESTÁNDAR 11 Ó 12)                     
    pwm = 0;                   //VELOCIDAD(valor 11) = 40 mm cada 3 sg
    //  PORTD ^= 7 << X_STP;
    if(Flag_Vertical){
      PORTD ^= 1 << Y_STP;
      cuentas_y++;
    }
    else{
      PORTD |= 1 << Y_STP;
    }
    if(Flag_Horizontal){
      PORTD ^= 1 << X_STP;
      cuentas_x++;
    }
    else{
      PORTD |= 1 << X_STP;
    }
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\r') {
      stringComplete = true;
    }
  }
}

void setup() {

  //LED de debug
  pinMode(13,OUTPUT);

  // Setup de las controladoras
  pinMode(X_DIR, OUTPUT); 
  pinMode(X_STP, OUTPUT);
  pinMode(Y_DIR, OUTPUT); 
  pinMode(Y_STP, OUTPUT);
  pinMode(Z_DIR, OUTPUT); 
  pinMode(Z_STP, OUTPUT);
  pinMode(EN, OUTPUT);
  // digitalWrite(EN, LOW);
  digitalWrite(EN, HIGH);

  // Setup de las ventosas (LED disponibilidad)
  DDRB |= B11111 << 1;          //Estas 2 líneas hacen lo 
  // PORTB |= B11111 << 1;         //mismo que las 10 de abajo
  // pinMode(9,OUTPUT);
  // pinMode(10,OUTPUT);
  // pinMode(11,OUTPUT);
  // pinMode(12,OUTPUT);
  // pinMode(13,OUTPUT);
  // digitalWrite(9, HIGH);
  // digitalWrite(10, HIGH);
  // digitalWrite(11, HIGH);
  // digitalWrite(12, HIGH);
  // digitalWrite(13, HIGH);
  

  //Setup de los Endstop
  pinMode(UP_V,INPUT_PULLUP);
  pinMode(UP_H,INPUT_PULLUP);
  pinMode(DOWN_V,INPUT_PULLUP);
  pinMode(DOWN_H,INPUT_PULLUP);

  config_Timer0();

  Serial.begin(115200);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

void loop() {
  //Analizamos las transiciones que haya en la máquina de estados
  Maquina_Estados();
   //Bucle de cambio de estado
  switch(Estado){
    case ST_INICIAL:
      CMD_Inicial();
    break;
    case ST_PARADO:
      CMD_Parado();
    break;
    case VERTICAL:
      CMD_Vertical();
    break;
    case HORIZONTAL:
      CMD_Horizontal();
    break;
    default:
      CMD_Parado();
      Instruccion_Codigo = ST_PARADO;
    break;
  }
}

