#ifndef __INIT_H
#define __INIT_H
#include "inttypes.h"
void init_all();
int16_t DS18B20_Read(void);
void DS18B20_Convert(void);  
void   DIR_1WIRE_IN();
void   DIR_1WIRE_OUT(); 
#endif
