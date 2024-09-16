/**
 * @file main.c
 * @author Corey Maxwell (corey.maxwell21@proton.me)
 * @brief Communication Between the MCP9808 and an ATMega328p
 * @version 0.1
 * @date 2024-09-13
 *
 * @copyright Copyright (c) 2024
 *
 */
/*
####################################################
System Includes
####################################################
*/
#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <util/delay.h>
/*
####################################################
Local Includes
####################################################
*/
#include "../include/USART.h"
/*
####################################################
Macros
####################################################
*/
#define DELAY_MS 1000L
#define SCL_SPEED 100000UL
#define INTERNAL_PULL_UP 1
#define BITRATE(TWSR)                                                                                                  \
    (uint8_t)(((F_CPU / SCL_SPEED) - 16) / (2 * (uint16_t)pow(4, (TWSR & ((1 << TWPS0) | (1 << TWPS1))))))
/*
####################################################
Function Prototypes
####################################################
*/
void i2c_init(void);
uint8_t i2c_start(void);
void i2c_stop(void);
/*
TODO :
    Implement both of these fucntions
        i2c_write -> writes data to the i2c bus
        i2c_read -> reads data from the i2c bus
        i2c_stop -> stop condition
    These can then be implemented within other higher level functions to allow for ease of use
    (maybe rename the functions to show that they are supposed to be private)

    There is some information regarding the code implementation within the TWI section of the datasheet
*/
uint8_t i2c_write(uint8_t data);
char i2c_read_ACK(void);
char i2c_read_NACK(void);
/*
####################################################
Global Variables
####################################################
*/
/*
####################################################
Main
####################################################
*/
int main(void)
{
    /* Initialise USART */
    USART_init(UBRR_VALUE);
    i2c_init();
    /*
    Set stdout to the &USART_stdout variable provided in the header
    Needed for printf to transmit over USART (UART)
    */
    stdout = &USART_stdout;
    while (1)
    {
        i2c_start();
        i2c_write(0b00100000);
		i2c_stop();
		i2c_start();
        i2c_write(0b00100000);
        char data;
        data = i2c_read_NACK();
        i2c_stop();
        printf("0x%X\n", data);
		_delay_ms(DELAY_MS);
    }
//     while (1)
//     {

//         //      printf("TWBR = %u of size %u bytes\n",BITRATE(TWSR = (0x00 & 0x03)), sizeof(TWSR));
//         printf("TWBR = %u\n", TWBR);
//         _delay_ms(DELAY_MS);
//         printf("Error Code : %u\n", i2c_start());
//         _delay_ms(DELAY_MS);
//     }
}

/*
####################################################
Function Definitions
####################################################
*/
void i2c_init(void)
{
    if (INTERNAL_PULL_UP)
    {
        DDRC |= (1 << PC4) | (1 << PC5);
        PORTC |= (1 << PC4) | (1 << PC5);
    }
    /* BITRATE is a C macro so the value of TWBR is calculated at compile time */
    TWBR = BITRATE(TWSR = 0x00);
}

void i2c_stop(void)
{
	printf("Stop\n");
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    while ((TWCR & (1 << TWSTO)))
        ;
}

uint8_t i2c_start(void)
{
	printf("Start\n");
    /* Sends Start Flag */
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    /* Wait until the start signal is sent */
    while (!(TWCR & (1 << TWINT)))
        ;
    return ((TWSR & 0xF8) == 0x08) ? 0 : 1;
}
uint8_t i2c_write(uint8_t data)
{
	printf("Write\n");
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    /* ACK Recieved */
    if ((TWSR & 0xF8) != 0x18)
    {
        return 0;
    }
    /* NACK Recieved */
    if ((TWSR & 0xF8) != 0x20)
    {
        return 1;
    }
    /* Failed */
    return 2;
}
char i2c_read_ACK(void)
{
	printf("Read Ack\n");
    TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)))
        ;
    return TWDR;
}
char i2c_read_NACK(void)
{
	printf("Read Nack\n");
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)))
        ;
    return TWDR;
}