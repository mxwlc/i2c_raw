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
#include <stdbool.h>
#include <stdio.h>
#include <util/delay.h>
#include <util/twi.h>
/*
####################################################
Local Includes
####################################################
*/
#include "../include/USART.h"
/*
####################################################
Typedef
####################################################
*/
typedef uint8_t i2c_error_ret;
/*
####################################################
Defines
####################################################
*/
#define DELAY_MS 1000L
#define SCL_SPEED 100000UL
#define INTERNAL_PULL_UP 1
#define SDA_PIN PC4
#define SCL_PIN PC5

/*
####################################################
Macros
####################################################
*/
#define BITRATE(TWSR)                                                                                                  \
    (uint8_t)(((F_CPU / SCL_SPEED) - 16) / (2 * (uint16_t)pow(4, (TWSR & ((1 << TWPS0) | (1 << TWPS1))))))
#define SLA_ADDR_W(ADDR) ((ADDR << 1) | TW_WRITE)
#define SLA_ADDR_R(ADDR) ((ADDR << 1) | TW_READ)
/*
####################################################
Function Prototypes
####################################################
*/
void i2c_init(void);
i2c_error_ret i2c_start(void);
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
i2c_error_ret i2c_write(uint8_t data);
unsigned char i2c_read(bool read_ack);
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
    // while (1)
    // {
    //     i2c_start();
    //     i2c_write(0b00100000);
    //     i2c_stop();
    //     i2c_start();
    //     i2c_write(0b00100000);
    //     char data;
    //     data = i2c_read_NACK();
    //     i2c_stop();
    //     printf("0x%X\n", data);
    //     _delay_ms(DELAY_MS);
    // }
    while (1)
    {
    }
}
/*
 ####################################################
 Function Definitions
 ####################################################
 */
void i2c_init(void)
{
    DDRC |= (1 << SDA_PIN) | (1 << SCL_PIN);
	
#if INTERNAL_PULL_UP
    /* Pulls SDA and SCL up to 3v3*/
    PORTC |= (1 << SDA_PIN) | (1 << SCL_PIN);

#else
    /* Resets SDA and SCL -> A & ~A = 0*/
    PORTC &= ~((1 << SDA_PIN) | (1 << SCL_PIN));
#endif
    // DDRC &= ~((1 << SDA_PIN) | (1 << SCL_PIN));
    /* BITRATE is a C macro so the value of TWBR is calculated at compile time */
    TWBR = BITRATE(TWSR = 0x00);
}

void i2c_stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    while ((TWCR & (1 << TWSTO)))
        ;
}

i2c_error_ret i2c_start(void)
{
    /* Sends Start Flag */
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    /* Wait until the start signal is sent */
    while (!(TWCR & (1 << TWINT)))
        ;
    if (TW_STATUS != TW_START && TW_STATUS != TW_REP_START)
    {
        return TW_STATUS;
    }
    return 0;
}
i2c_error_ret i2c_write_sla(uint8_t sla_addr)
{
    TWDR = sla_addr;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    /* ACK Recieved */
    if (TW_STATUS != TW_MT_SLA_ACK && TW_STATUS != TW_MT_SLA_NACK)
    {
        return TW_STATUS;
    }
    return 0;
}

i2c_error_ret i2c_write(uint8_t data){
	TWDR = data;
    while (!(TWCR & (1 << TWINT)))
        ;
	if (TW_STATUS != TW_MT_DATA_ACK){
		return TW_STATUS;
	}
	return 0;
}

unsigned char i2c_read(bool read_ack)
{
    if (read_ack)
    {
        TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
        while (!(TWCR & (1 << TWINT)))
            ;
        if (TW_STATUS != TW_MR_DATA_ACK)
        {
            return TW_STATUS;
        }
    }
    else
    {
        TWCR = (1 << TWEN) | (1 << TWINT);
        while (!(TWCR & (1 << TWINT)))
            ;
        if (TW_STATUS != TW_MR_DATA_NACK)
        {
            return TW_STATUS;
        }
    }
    uint8_t data = TWDR;
    return data;
}
