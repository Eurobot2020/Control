
#include "lpc17xx.h"
#define Gray 0xF79E

extern struct t_screenZone zone_1b;
extern struct t_screenZone zone_1c;
extern struct t_screenZone zone_1d;
extern struct t_screenZone zone_2b;
extern struct t_screenZone zone_2c;
extern struct t_screenZone zone_2d;
extern struct t_screenZone zone_3b;
extern struct t_screenZone zone_3c;
extern struct t_screenZone zone_3d;
extern struct t_screenZone zone_3e;
extern struct t_screenZone zone_3f;

void screenMain(void);
int8_t zonePressed(struct t_screenZone* zone);
int8_t zoneNewPressed(struct t_screenZone* zone);
void squareButton(struct t_screenZone* zone, char * text, uint16_t textColor, uint16_t lineColor, uint16_t backColor);
void fillRect(struct t_screenZone* zone, uint16_t color);
