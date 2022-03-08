#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bme280.h"
#include "control.h"
#include "gpio.h"
#include "pid.h"
#include "uart.h"
#include "thermometer.h"

#define true 1
#define false 0

int use_terminal = false;
struct bme280_dev bme_connection;
int uart_filesystem, key_gpio = 1;

void shutdown_program() {
  system("clear");
  printf("Programa encerrado\n");
  turn_resistance_off();
  turn_fan_off();
  close_uart(uart_filesystem);
  exit(0);
}

void potentiometer_routine(int key) {
  system("clear");
  float TI, TR, TE;
  int value_to_send = 0;
  printf("\n Controle Potenciometro \n");
  pid_configura_constantes(20, 0.1, 100);
  do {
    write_uart_get(uart_filesystem, TEMPERATURA);
    TI = read_uart(uart_filesystem, TEMPERATURA).float_value;

    double value_to_send = pid_controle(TI);

    pwm_control(value_to_send);

    write_uart_get(uart_filesystem, POTENCIOMETRO);
    TR = read_uart(uart_filesystem, POTENCIOMETRO).float_value;

    pid_atualiza_referencia(TR);

    TE = get_current_temperature(&bme_connection);
    printf("\tUART TI: %.2f⁰C - TR: %.2f⁰C - TE: %.2f⁰C\n", TI, TR, TE);
    print_lcdTEMP("UART ", TI, TR, TE);

    if (!use_terminal) {
      write_uart_get(uart_filesystem, COMANDO);
      key_gpio = read_uart(uart_filesystem, COMANDO).int_value;
    }

    write_uart_send_CTR(uart_filesystem, value_to_send);
  } while (key_gpio == key);
  printf("\n");
}

void terminal_routine(float TR, int key) {
  system("clear");
  float TI, TE;
  int value_to_send = 0;
  printf("\n Controle Terminal \n");
  pid_configura_constantes(20, 0.1, 100);
  do {
    write_uart_get(uart_filesystem, TEMPERATURA);
    TI = read_uart(uart_filesystem, TEMPERATURA).float_value;

    double value_to_send = pid_controle(TI);

    pwm_control(value_to_send);

    pid_atualiza_referencia(TR);

    TE = get_current_temperature(&bme_connection);
    printf("\tTERMINAL TI: %.2f⁰C - TR: %.2f⁰C - TE: %.2f⁰C\n", TI, TR, TE);
    print_lcdTEMP("Terminal ", TI, TR, TE);

    if (!use_terminal) {
      write_uart_get(uart_filesystem, COMANDO);
      key_gpio = read_uart(uart_filesystem, COMANDO).int_value;
    }

    write_uart_send_REF(uart_filesystem, TR);
  } while (key_gpio == key);
  printf("\n");
}

void init() {
  wiringPiSetup();
  turn_resistance_off();
  turn_fan_off();
  lcd_init();
  bme_connection = connect_bme();
  uart_filesystem = connect_uart();
  system("clear");
}

void menu () {
  int option;
  float INPUT_TR;
  printf("Escolha uma opção de controle: \n\t1)Potenciometro \n\t2) Terminal\n\t\n");
  scanf("%d", &option);
  if (option==2) {
      system("clear");
      printf("Digite o valor que deseja para a Temperatura Referencial\n");
      scanf("%f", &INPUT_TR);
      use_terminal = true;
      terminal_routine(INPUT_TR, 1);
    else if(option==1){
      use_terminal = true;
      potentiometer_routine(1);
    }
    else{
      system("clear");
      printf("Opção invalida\n");
      menu();
    }
  }
}

int main() {
  init();
  signal(SIGINT, shutdown_program);
  menu();
  return 0;
}