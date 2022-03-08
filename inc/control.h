#ifndef _DISPLAY_H_
#define _DISPLAY_H_


#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>

#define I2C_ENDERECO   0x27 

#define ENVIAR_DATA  1 
#define ENVIAR_COMANDO  0 

#define LINHA1  0x80 
#define LINHA2  0xC0 

#define LUZ_DE_FUNDO   0x08  

#define ENABLE  0b00000100 

void iniciar_lcd(void);
void lcd_byte(int bits, int mode);
void lcd_enable(int bits);

void typeInt(int i);
void typeFloat(float myFloat);
void lcdLoc(int line); 
void ClrLcd(void); 
void typeln(const char *s);
void typeChar(char val);

void imprimir_temp(char [],float tempI, float tempR, float tempE);

#endif