#include "uart.h"
#include "crc16.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  //Used for UART
#include <fcntl.h>   //Used for UART
#include <termios.h> //Used for UART
#include "controller.h"

void init_uart(int *uart_filestream)
{
    setup_start(uart_filestream);
    configure_uart(uart_filestream);
    set_uart_filestream(uart_filestream);
}

void setup_start(int *uart_filestream)
{
    printf("Opening UART... ");

    *uart_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (*uart_filestream == -1)
    {
        printf("Error: Unable to open UART.\n");
        printf("Shutting down\n");
        exit(1);
    }
    else
    {
        printf("UART started.\n");
    }
}

void configure_uart(int *uart_filestream)
{
    printf("Configuring UART... ");
    struct termios options;
    tcgetattr(*uart_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(*uart_filestream, TCIFLUSH);
    tcsetattr(*uart_filestream, TCSANOW, &options);
    printf("✓\n");
}

void close_uart(int *uart_filestream)
{
    printf("Shutting down... ");
    close(*uart_filestream);
    printf("Finished.\n");
}

int write_uart(int *uart_filestream, unsigned char *message, int size)
{
    unsigned char tx_buffer[200];
    short crc = calcula_CRC(message, size);
    memcpy(tx_buffer, message, size);
    memcpy(&tx_buffer[size], &crc, 2);

    // printf("Writing... ");
    int count = write(*uart_filestream, tx_buffer, size + 2); // Filestream, bytes to write, number of bytes to write
    if (count < 0)
    {
        printf("UART Write error - TX error\n");
        return -1;
    }
    else
    {
        // printf("Success.\n");
    }
    return 0;
}

int check_rx_return(int rx_length)
{
    if (rx_length < 0)
    {
        // An error occured (will occur if there are no bytes)
        printf("UART Read error - RX error\n");
        return -1;
    }
    else if (rx_length == 0)
    {
        // No data waiting
        printf("No data.\n");
        return 0;
    }
    else
    {
        // Bytes received
        // printf("%i bytes read \n", rx_length);
        return rx_length;
    }
}

int read_bytes(int *uart_filestream, void *buffer, int size)
{
    int rx_length = -1;
    int receive_counter = 0;
    int try = 0;
    while (receive_counter < size)
    {
        rx_length = read(*uart_filestream, buffer, 1);
        if (rx_length > 0)
        {
            receive_counter++;
            buffer++;
        }
        else
        {
            usleep(10000);
            try++;
        }
        if (try > 10)
            return -1;
    }
    return size;
}

int read_data(int *uart_filestream, unsigned char *code, void *data, int size)
{
    unsigned char rx_buffer[255];
    short crc_received;
    short crc_computed;
    int received_counter = 0;
    int message_body = 1;

    while (message_body)
    {
        switch (received_counter)
        {
        case 0:
            if (read_bytes(uart_filestream, &rx_buffer[received_counter], 1) && (rx_buffer[received_counter] == CLIENTE))
            {

                received_counter++;
            }
            break;
        case 1:
            if (read_bytes(uart_filestream, &rx_buffer[received_counter], 1) && (rx_buffer[received_counter] == code[1]))
            {

                received_counter++;
            }
            else
            {
                received_counter = 0;
            }
            break;
        case 2:
            if (read_bytes(uart_filestream, &rx_buffer[received_counter], 1) && (rx_buffer[received_counter] == code[2]))
            {

                received_counter++;
            }
            else
            {
                received_counter = 0;
            }
            break;
        case 3:
            if (read_bytes(uart_filestream, &rx_buffer[received_counter], size))
            {
                memcpy(data, &rx_buffer[received_counter], size);
                received_counter++;
            }
            else
            {
                received_counter = 0;
            }
            break;
        case 4:
            if (read_bytes(uart_filestream, &crc_received, 2))
            {
                crc_computed = calcula_CRC(rx_buffer, size + 3);
                if (crc_computed == crc_received)
                {

                    return 0;
                }
                else
                {

                    return -1;
                }
            }
            else
            {
                return -1;
            }
            break;
        default:

            return -1;
            break;
        }
    }
    return 0;
}

void debug(unsigned char *msg, int size)
{
    printf("Message received: ");
    for (int i = 0; i < size; i++)
    {
        printf("0x%x ", msg[i]);
    }
    printf("\n");
}

//
float read_float(int *uart_filestream)
{
    // Read up to 255 characters from the port if they are there
    unsigned char rx_buffer[100];
    int rx_length = read(*uart_filestream, (void *)rx_buffer, 9); // Filestream, buffer to store in, number of $

    printf("Read FLOAT [%d]\n", rx_length);

    int response = check_rx_return(rx_length);
    if (response < 0)
        return 0;
    else if (response == 0)
        return 0;
    else if (response < 9)
    {
        usleep(50000);
        unsigned char rx_buffer_2[100];
        int rx_length_2 = read(*uart_filestream, (void *)rx_buffer_2, 9 - rx_length);
        printf("No data.\n"); // No data waiting

        if (rx_length_2 == 9 - rx_length)
        {
            memcpy(&rx_buffer[rx_length], rx_buffer_2, rx_length_2);
            debug(rx_buffer, rx_length + rx_length_2);
            float data = 0;
            memcpy(&data, &rx_buffer[3], 4);
            return data;
        }
        else
        {
            rx_length = read(*uart_filestream, (void *)rx_buffer, 100);
            return 0;
        }
    }
    else
    {
        debug(rx_buffer, rx_length);
        float data = 0;
        memcpy(&data, &rx_buffer[3], 4);
        return data;
    }
}

int read_int(int *uart_filestream)
{
    // Read up to 255 characters from the port if they are there
    unsigned char rx_buffer[100];
    int rx_length = read(*uart_filestream, (void *)rx_buffer, 9); // Filestream, buffer to store in, number of $

    int response = check_rx_return(rx_length);
    if (response < 0)
        return 0;
    else if (response == 0)
        return 0;
    else
    {
        debug(rx_buffer, rx_length);
        int dado = 0;
        memcpy(&dado, &rx_buffer[3], 4);
        return dado;
    }
}

char read_char(int *uart_filestream)
{
    // Read up to 255 characters from the port if they are there
    unsigned char rx_buffer[100];
    int rx_length = read(*uart_filestream, (void *)rx_buffer, 6); // Filestream, buffer to store in, number of $

    int response = check_rx_return(rx_length);
    if (response < 0)
        return 0;
    else if (response == 0)
        return 0;
    else
    {
        debug(rx_buffer, rx_length);
        char dado = 0;
        memcpy(&dado, &rx_buffer[3], 1);
        return dado;
    }
}

int read_string(int *uart_filestream, char *message)
{
    // Read up to 255 characters from the port if they are there
    unsigned char rx_buffer[100];
    int rx_length = read(*uart_filestream, (void *)rx_buffer, 100); // Filestream, buffer to store in, number of $

    int response = check_rx_return(rx_length);
    if (response < 0)
        return 0;
    else if (response == 0)
        return 0;
    else
    {
        debug(rx_buffer, rx_length);
        int size = (int)rx_buffer[3];
        memcpy(message, &rx_buffer[4], size);
        message[size] = '\0';
        return 1; //
    }
}