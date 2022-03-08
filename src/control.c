#include "control.h"

int fd;  

void imprimir_temp(char MODE[],float tempI, float tempR, float tempE) {

  if (wiringPiSetup () == -1) exit (1);

  fd = wiringPiI2CSetup(I2C_ENDERECO);

  iniciar_lcd(); 
  lcdLoc(LINHA1);
  typeln(MODE);
  typeln("TI:");
  typeFloat(tempI);
  lcdLoc(LINHA2);
  typeln("TR:");
  typeFloat(tempR);
  typeln(" TE:");
  typeFloat(tempE);

}


void typeFloat(float myFloat)   {
  char buffer[20];
  sprintf(buffer, "%4.2f",  myFloat);
  typeln(buffer);
}

void typeInt(int i)   {
  char array1[20];
  sprintf(array1, "%d",  i);
  typeln(array1);
}

void ClrLcd(void)   {
  lcd_byte(0x01, ENVIAR_COMANDO);
  lcd_byte(0x02, ENVIAR_COMANDO);
}

void lcdLoc(int line)   {
  lcd_byte(line, ENVIAR_COMANDO);
}

void typeChar(char val)   {

  lcd_byte(val, ENVIAR_DATA);
}


void typeln(const char *s)   {

  while ( *s ) lcd_byte(*(s++), ENVIAR_DATA);

}

void lcd_byte(int bits, int mode)   {


  int bits_high;
  int bits_low;
  bits_high = mode | (bits & 0xF0) | LUZ_DE_FUNDO ;
  bits_low = mode | ((bits << 4) & 0xF0) | LUZ_DE_FUNDO ;

  wiringPiI2CReadReg8(fd, bits_high);
  lcd_enable(bits_high);

  wiringPiI2CReadReg8(fd, bits_low);
  lcd_enable(bits_low);
}

void lcd_enable(int bits)   {
  delayMicroseconds(500);
  wiringPiI2CReadReg8(fd, (bits | ENABLE));
  delayMicroseconds(500);
  wiringPiI2CReadReg8(fd, (bits & ~ENABLE));
  delayMicroseconds(500);
}


void iniciar_lcd()   {
  lcd_byte(0x33, ENVIAR_COMANDO); // Initialise
  lcd_byte(0x32, ENVIAR_COMANDO); // Initialise
  lcd_byte(0x06, ENVIAR_COMANDO); // Cursor move direction
  lcd_byte(0x0C, ENVIAR_COMANDO); // 0x0F On, Blink Off
  lcd_byte(0x28, ENVIAR_COMANDO); // Data length, number of lines, font size
  lcd_byte(0x01, ENVIAR_COMANDO); // Clear display
  delayMicroseconds(500);
}