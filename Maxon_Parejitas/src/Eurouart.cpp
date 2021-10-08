
#include "Eurouart.h" //Enlazo con la biblioteca
#include <Arduino.h>
#include "variables.h"

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

void(* resetFunc) (void) = 0;

void Traduccion_Variables(cinematica *variable)       
{ //Se encarga de leer el mensaje recibido, actualizar las variables y levantar los flags.
  Serial.println(inputString); 
  Instruccion_Codigo = T_INSTRUCCION;
  // Serial.println(Instruccion_Codigo);
    switch (Instruccion_Codigo)
    {
    case ('G'):
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
            Serial.println("ES");
            Instruccion_Codigo = 1;
        }

        break;
    case ('D'):
        Instruccion_Prioridad = BIT_PRIO-'0';
        distancia = (BIT_M_DISTANCIA - '0') * 1000 + (BIT_C_DISTANCIA - '0') * 100 + (BIT_D_DISTANCIA - '0') * 10 + (BIT_U_DISTANCIA - '0');
				variable->distancia=distancia;
        velocidad_final = (BIT_M_V_FINAL - '0') * 1000 + (BIT_C_V_FINAL - '0') * 100 + (BIT_D_V_FINAL - '0') * 10 + (BIT_U_V_FINAL - '0');
        velocidad_maxima = (BIT_M_VMAX - '0') * 1000 + (BIT_C_VMAX - '0') * 100 + (BIT_D_VMAX - '0') * 10 + (BIT_U_VMAX - '0');
        // Serial.println(distancia);
        if (BIT_SIGNO_GRADOS == '+')
        {
            distancia = distancia;
        }
        else if (BIT_SIGNO_GRADOS == '-')
        {
            distancia = -1 * distancia;
        }
        else
        {
            Serial.println("ES");
            Instruccion_Codigo = 1;
        }
        break;
    case ('C'):
        Instruccion_Prioridad = BIT_PRIO-'0';
        distancia = (BIT_M_DISTANCIA_C - '0') * 1000 + (BIT_C_DISTANCIA_C - '0') * 100 + (BIT_D_DISTANCIA_C - '0') * 10 + (BIT_U_DISTANCIA_C - '0');
        velocidad_final = (BIT_M_V_FINAL_C - '0') * 1000 + (BIT_C_V_FINAL_C - '0') * 100 + (BIT_D_V_FINAL_C - '0') * 10 + (BIT_U_V_FINAL_C - '0');
        velocidad_maxima = (BIT_M_VMAX_C - '0') * 1000 + (BIT_C_VMAX_C - '0') * 100 + (BIT_D_VMAX_C - '0') * 10 + (BIT_U_VMAX_C - '0');
        radio = (BIT_M_RADIO_C - '0') * 1000 + (BIT_C_RADIO_C - '0') * 100 + (BIT_D_RADIO_C - '0') * 10 + (BIT_U_RADIO_C - '0');
        break;

        //Instruccion_Prioridad = BIT_PRIO - '0'; //URGENTE PARAR
    case ('F'):
        Instruccion_Prioridad = 1; 
        // Serial.println("Frenaaa");
				break;
		
		case ('S'):		//REQUEST DEL ESTADO POR NIVEL ALTO
				Request = 1;
        Instruccion_Codigo = 1;
        Instruccion_Prioridad = 0;
        // Serial.println("Request");
				break;
		case ('R'):		//RESET POR UART
				resetFunc();

				break;
		
    default:
        Serial.println("EI");
        Instruccion_Codigo = 1;
    }
  // clear the string:
  inputString = "";  // Importante inicializar las Strings (OJO A LA MAYÚSCULA) o luego nos reímos
}

