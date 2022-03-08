#ifndef UART_H_
#define UART_H_

#include <unistd.h>  //Used for UART
#include <fcntl.h>   //Used for UART
#include <termios.h> //Used for UART

#define MATRICULA 0x02, 0x06, 0x00, 0x06

#define CLIENTE 0x00
#define SERVIDOR 0x01
#define CMD 0x23
#define ENVIAR_COMANDO 0x16

#define TEMPERATURA 0xC1
#define POTENCIOMETRO 0xC2
#define COMANDO 0xC3

#define CONTROLE_SINAL 0xD1
#define REF_SINAL 0xD2
#define SISTEMA_STATUS 0xD3
#define REF_MODO 0xD4

#define LIGAR_CMD 1
#define DESLIGAR_CMD 2
#define REF_POTENCIOMETRO 3
#define REF_CURVA 4

#define ESPERA 500000

// SETUP UART
void init_uart(int *);

// OPEN SERIAL PORT
void setup_start(int *);

// CONFIGURE THE UART
// The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
void configure_uart(int *);

// CLOSE UART
void close_uart(int *);

// TX BYTES
int write_uart(int *uart_filestream, unsigned char *message, int size);

// CHECK FOR ANY RX BYTES
int read_data(int *uart_filestream, unsigned char *code, void *data, int size);
int read_int(int *uart_filestream);
float read_float(int *uart_filestream);
char read_char(int *uart_filestream);
int read_string(int *uart_filestream, char *message);

#endif