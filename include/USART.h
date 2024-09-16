/**
 * @file USART.h
 * @author your name (you@domain.com)
 * @brief ATMega328p USART Drivers
 * @version 0.1
 * @date 2024-09-13
 * Simple USART functionality fro ATMega328p
 * Baud Rate : 9600
 * Clock oscillation : 16 MHz
 * Operating Mode : Asynchronous (U2Xn=0)
 * @copyright Copyright (c) 2024
 *
 */
#ifndef USART_H_
#define USART_H_
#include <avr/io.h>
#include <stdio.h>
/*
####################################################
Macros
####################################################
*/

#define FOSC 16000000UL
#define BAUD 9600L
#define UBRR_VALUE ((unsigned int)(FOSC / (BAUD * 16) - 1))
/*
####################################################
Function Prototypes
####################################################
*/

/**
 * @brief Initialises USART
 * Loads the Desired Baud rate into the UBRR0H and UBRR0L registers and enames the both the reciever and transmitter
 * functionality.
 *
 * @param ubrr 16 bit desired Baud Rate
 */
void USART_init(unsigned int ubrr);

/**
 * @brief Transmits a character over USART
 * Waits for the USART data register to be empty and then loads a character into the correct register for the data to be
 * transmitted then.
 *
 * @param data Byte of data to be transmitted over USART
 */
void USART_put_char(unsigned char data);

/**
 * @brief Transmits a string of characters over USART
 * Iterates through a string and sends each character over USART.
 * @param str
 */
void USART_put_str(char *str);

/**
 * @brief Recieves a character from USART stream
 * Waits for the usart Recieve Flasg to be asserted and then returns the contents of the UDR0 register.
 *
 * @return The recieved character
 */
unsigned char USART_get_char(void);

/**
 * @brief Flushes the USART Registers
 * Assigns the contents of the dummy varible untill the RXC0 flag is no longer asserted.
 */
void USART_flush(void);

/**
 * @brief Allows for printf to transmit over USART 
 * 
 *
 * @param var character to be transmitted 
 * @param stream stdio Stream 
 * @return Error code
 */
int USART_put_char_printf(char var, FILE *stream);
/*
####################################################
Global Variables
####################################################
*/
static FILE USART_stdout = FDEV_SETUP_STREAM(USART_put_char_printf, NULL, _FDEV_SETUP_WRITE);
#endif // USART_H_