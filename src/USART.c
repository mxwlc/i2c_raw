#include "../include/USART.h"

/*
####################################################
Function Declerations
####################################################
*/

void USART_init(unsigned int ubrr)
{
    // Loads BAUD Rate into UBRR0H (upper 8 bits) and UBRR0L (Lower 8 bits)
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr & 0xFF);

    // Enable Reciever and Transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

void USART_put_char(unsigned char data)
{
    /* Wait for USART data register to be empty */
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    // Put data into data register
    UDR0 = data;
}

void USART_put_str(char *str)
{
    /* Iterate through string */
    while (*str)
    {
        USART_put_char(*str);
        str++;
    }
}

unsigned char USART_get_char(void)
{
	/* Wait until the USART Recieve Flag is asserted and then return the contents of the data register */
    while (!(UCSR0A & (1 << RXC0)))
        ;
    return UDR0;
}

void USART_flush(void)
{
	/* Flush USART Registers */
    unsigned char dummy;
    while (UCSR0A & (1 << RXC0))
        dummy = UDR0;
}

int USART_put_char_printf(char var, FILE *stream)
{
	/* Transmits characters form the stdio stream over USART */
    if (var == '\n')
        USART_put_char('\r');
    USART_put_char(var);
    return 0;
}