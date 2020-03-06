/*****************************************
************** pantalla.c  ***************


*****************************************/

#include "GLCD.h"
#include "TouchPanel.h"
#include <string.h>

#define Gray 0xF79E

/* Variable que almacena el estado */
uint8_t screenState;   

/* Estructura que define una zona de la pantalla */
struct t_screenZone
{
  uint16_t x;         
	uint16_t y;
	uint16_t size_x;
	uint16_t size_y;
	uint8_t  pressed;
};

/* Definicion de las diferentes zonas de la pantalla */
struct t_screenZone zone_0  = { 20,  20, 200,  50, 0}; /* Título "PRUEBA POSAVASOS */
struct t_screenZone zone_1a = { 20,  80, 100,  30, 0}; /* Título "Eje X:" 		     */
struct t_screenZone zone_1b = {120,  80, 100,  30, 0}; /* Dato de pasos 		   	   */
struct t_screenZone zone_1c = { 50, 110,  50,  40, 0}; /* Botón "-"				 		     */
struct t_screenZone zone_1d = {140, 110,  50,  40, 0}; /* Botón "+"				 		     */
struct t_screenZone zone_2a = { 20, 160, 100,  30, 0}; /* Título "Eje Z:"				   */
struct t_screenZone zone_2b = {120, 160, 100,  30, 0}; /* Dato de pasos					   */
struct t_screenZone zone_2c = { 50, 190,  50,  40, 0}; /* Botón "-"  							 */
struct t_screenZone zone_2d = {140, 190,  50,  40, 0}; /* Botón "+"  						   */
struct t_screenZone zone_3a = { 20, 240, 200,  30, 0}; /* Título "Ventosas ()"  	 */
struct t_screenZone zone_3b = { 25, 270,  30,  30, 0}; /* Botón Electrovalvula 1   */
struct t_screenZone zone_3c = { 65, 270,  30,  30, 0}; /* Botón Electrovalvula 2   */
struct t_screenZone zone_3d = {105, 270,  30,  30, 0}; /* Botón Electrovalvula 3   */
struct t_screenZone zone_3e = {145, 270,  30,  30, 0}; /* Botón Electrovalvula 4   */
struct t_screenZone zone_3f = {185, 270,  30,  30, 0}; /* Botón Electrovalvula 5   */

/* Flag que indica si se detecta una pulsación válida */
uint8_t pressedTouchPanel = 0;


void squareButton(struct t_screenZone* zone, char * text, uint16_t textColor, uint16_t lineColor, uint16_t backColor)
/*Description    : Dibuja un cuadrado en las coordenadas especificadas colocando 
 *                  un texto en el centro del recuadro
 * Input          : zone: zone struct
 *                  text: texto a representar en el cuadro
 *                  textColor: color del texto
 *                  lineColor: color de la línea
 * Output         : None
 * Return         : None
 * Attention		  : None
 *******************************************************************************/
{
  LCD_DrawLine( zone->x, zone->y, zone->x + zone->size_x, zone->y, lineColor);
  LCD_DrawLine( zone->x, zone->y, zone->x, zone->y + zone->size_y, lineColor);
  LCD_DrawLine( zone->x, zone->y + zone->size_y, zone->x + zone->size_x, zone->y + zone->size_y, lineColor);
  LCD_DrawLine( zone->x + zone->size_x, zone->y, zone->x + zone->size_x, zone->y + zone->size_y, lineColor);
	GUI_Text(zone->x + zone->size_x/2 - (strlen(text)/2)*8, zone->y + zone->size_y/2 - 8,
            (uint8_t*) text, textColor, backColor);	
}

void fillRect(struct t_screenZone* zone, uint16_t color)
/*Description    : Rellena de un color determinado el interior de una zona 
 * Input          : zone: Estructura con la información de la zona
 *                  color: color de relleno
 * Output         : None
 * Return         : None
 * Attention		  : None
*******************************************************************************/
{
	uint16_t i;
	
	for (i = zone->y+1; i < zone->y + zone->size_y-1; i ++) {
		LCD_DrawLine(zone->x + 1, i, zone->x + zone->size_x -1, i, color);
	}
}

void drawMinus(struct t_screenZone* zone, uint16_t lineColor)
/*Description    : Draw a minus sign in the center of the zone
 * Input          : zone: zone struct
 *                  lineColor
 * Output         : None
 * Return         : None
 * Attention		  : None
 *******************************************************************************/
{
   LCD_DrawLine( zone->x + 5 , zone->y + zone->size_y/2 - 1, 
                 zone->x + zone->size_x-5, zone->y + zone->size_y/2 - 1,
                 lineColor);
   LCD_DrawLine( zone->x + 5 , zone->y + zone->size_y/2, 
                 zone->x + zone->size_x-5, zone->y + zone->size_y/2,
                 lineColor);
   LCD_DrawLine( zone->x + 5 , zone->y + zone->size_y/2 + 1, 
                 zone->x + zone->size_x-5, zone->y + zone->size_y/2 + 1,
                 lineColor);
}


void drawAdd(struct t_screenZone* zone, uint16_t lineColor)
/*Description    : Draw a add sign in the center of the zone
 * Input          : zone: zone struct
 *                  lineColor
 * Output         : None
 * Return         : None
 * Attention		  : None
 *******************************************************************************/
{
   drawMinus(zone, lineColor);
   
   LCD_DrawLine( zone->x + zone->size_x/2 - 1,  zone->y + 5 ,
                 zone->x + zone->size_x/2 - 1,  zone->y + zone->size_y - 5, 
                 lineColor);
   LCD_DrawLine( zone->x + zone->size_x/2 ,  zone->y + 5 ,
                 zone->x + zone->size_x/2 ,  zone->y + zone->size_y - 5, 
                 lineColor);
   LCD_DrawLine( zone->x + zone->size_x/2 + 1,  zone->y + 5 ,
                 zone->x + zone->size_x/2 + 1,  zone->y + zone->size_y - 5, 
                 lineColor);
}


void screenMain(void)
/* Description    : Visualiza la pantalla principal
 * Input          : None
 * Output         : None
 * Return         : None
 * Attention		  : None
 *******************************************************************************/
{
	LCD_Clear(Gray);
	squareButton(&zone_0, "PRUEBA POSAVASOS", Green, Gray, Black);
	squareButton(&zone_1a, "Eje X:", Black, Gray, Gray);
	squareButton(&zone_1b, "0", Black, Gray, Gray);
	squareButton(&zone_1c, "", Gray, Black, Gray);
	drawMinus(&zone_1c, Black);
	squareButton(&zone_1d, "", Gray, Black, Gray);
	drawAdd(&zone_1d, Black);
	squareButton(&zone_2a, "Eje Z:", Black, Gray, Gray);
	squareButton(&zone_2b, "0", Black, Gray, Gray);
	squareButton(&zone_2c, "", Gray, Black, Gray);
	drawMinus(&zone_2c, Black);
	squareButton(&zone_2d, "", Gray, Black, Gray);
	drawAdd(&zone_2d, Black);
	squareButton(&zone_3a, "Ventosas (o lo que querais)", Black, Gray, Gray);
	fillRect(&zone_3b, Black);
	fillRect(&zone_3c, Black);
	fillRect(&zone_3d, Black);
	fillRect(&zone_3e, Black);
	fillRect(&zone_3f, Black);
}

void checkTouchPanel(void)
/*Description    : Lee el TouchPanel y almacena las coordenadas si detecta pulsación
 * Input          : None
 * Output         : Modifica pressedTouchPanel
 *                    0 - si no se detecta pulsación
 *                    1 - si se detecta pulsación
 *                        En este caso se actualizan las coordinadas en la estructura display
 * Return         : None
 * Attention		  : None
 *******************************************************************************/
{
	Coordinate* coord;
	
	coord = Read_Ads7846();
	
	if (coord > 0) {
	  getDisplayPoint(&display, coord, &matrix );
     pressedTouchPanel = 1;
   }   
   else
     pressedTouchPanel = 0;
}

int8_t zonePressed(struct t_screenZone* zone)
/*Description    : Detecta si se ha producido una pulsación en una zona contreta
 * Input          : zone: Estructura con la información de la zona
 * Output         : Modifica zone->pressed
 *                    0 - si no se detecta pulsación en la zona
 *                    1 - si se detecta pulsación en la zona
 * Return         : 0 - si no se detecta pulsación en la zona
 *                  1 - si se detecta pulsación en la zona
 * Attention		  : None
 *******************************************************************************/
{
	if (pressedTouchPanel == 1) {

		if ((display.x > zone->x) && (display.x < zone->x + zone->size_x) && 
			  (display.y > zone->y) && (display.y < zone->y + zone->size_y))
      {
         zone->pressed = 1;
		   return 1;
      }   
	}
   
	zone->pressed = 0;
	return 0;
}

int8_t zoneNewPressed(struct t_screenZone* zone)
/*Description    : Detecta si se ha producido el flanco de una nueva pulsación en 
 *                  una zona contreta
 * Input          : zone: Estructura con la información de la zona
 * Output         : Modifica zone->pressed
 *                    0 - si no se detecta pulsación en la zona
 *                    1 - si se detecta pulsación en la zona
 * Return         : 0 - si no se detecta nueva pulsación en la zona
 *                  1 - si se detecta una nueva pulsación en la zona
 * Attention		  : None
 *******************************************************************************/
{
	if (pressedTouchPanel == 1) {

		if ((display.x > zone->x) && (display.x < zone->x + zone->size_x) && 
			  (display.y > zone->y) && (display.y < zone->y + zone->size_y))
      {
         if (zone->pressed == 0)
         {   
            zone->pressed = 1;
            return 1;
         }
		   return 0;
      }
	}

   zone->pressed = 0;
	return 0;
}




