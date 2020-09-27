/*
 * Clock_FW.c
 *
 * Created: 25.9.2020 14:34:15
 * Author : Martin Košút
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "twi.h"
//#include "ds3231.h"
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>


#define UART_BAUD_RATE	9600
#define RTC_address		0x68
#define EEPROM_address	0x57
#define btnDOWN			PORTD4
#define btnUP			PORTD5
#define btnOK_MENU		PORTD6
#define btnEXIT			PORTD7

typedef struct Time_data
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t date;
	uint8_t month;
	uint8_t year;
}T_Time_data;
/*
void fsm_twi_scanner(void);
typedef enum
{
	IDLE_STATE = 1,
	TRY_STATE,
	ACK_STATE
} state_t;
state_t current_state = IDLE_STATE;
*/
uint8_t dec2bcd(char num);
uint8_t bcd2dec(char num);

T_Time_data data;

int main(void)
{
	DDRC |= (1<<PORTC5) | (1<<PORTC4);
	DDRD |= (1<<PORTD0) | (1<<PORTD1);
	DDRD &= ~((1<<btnDOWN) | (1<<btnUP) | (1<<btnOK_MENU) | (1<<btnEXIT));		//input pins
	PORTD |= (1<<btnDOWN) | (1<<btnUP) | (1<<btnOK_MENU) | (1<<btnEXIT);		//turn on pull up resistors
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
	twi_init();
	sei();
	
	uart_puts("\r\nstart\r\n");
	_delay_ms(3000);
	
	uint8_t status, i;
	char read_data[10];
	char uart_string[20];
	
	while (1) 
    {
		status = twi_start((RTC_address<<1) + TWI_WRITE);
		if (status == 0)
		{
			twi_write(0x00);
		}
		twi_stop();
		
		status = twi_start((RTC_address<<1) + TWI_READ);
		if (status == 0)
		{
			for (i=0; i<6; i++)
			{
				read_data[i] = twi_read_ack();
			}
			read_data[i] = twi_read_nack();
		}
		twi_stop();
		
		
		data.seconds = bcd2dec(read_data[0]);
		data.minutes = bcd2dec(read_data[1]);
		data.hours = bcd2dec(read_data[2]);
		data.date = bcd2dec(read_data[4]);
		data.month = bcd2dec(read_data[5]);
		data.year = bcd2dec(read_data[6]);
		sprintf(uart_string, "%d.%d.%d %d:%d:%d\r\n", data.date, data.month, data.year, data.hours, data.minutes, data.seconds);
		
		//itoa(data.hours, uart_string, 10);
		uart_puts(uart_string);
		//uart_puts("\r\n");
		_delay_ms(1000);
	
    }
}

uint8_t dec2bcd(char num)
{
	return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal

uint8_t bcd2dec(char num)
{
	return ((num/16 * 10) + (num % 16));
}
/*
void fsm_twi_scanner(void)
{
	static uint8_t addr = 0;
	uint8_t status;
	char uart_string[3];

	switch (current_state)
	{
		case IDLE_STATE:
		if (addr < 128)
		{
			itoa(addr, uart_string, 16);
			uart_puts("\r\n");
			uart_puts(" ");
			uart_puts(uart_string);
			current_state = TRY_STATE;
		}
		else
		{
			_delay_ms(1000);
			addr = 0;
			current_state = IDLE_STATE;
			uart_puts("\r\n");
		}
		break;

		// Transmit TWI slave address and check status
		case TRY_STATE:
		status = twi_start((addr<<1) + TWI_WRITE);
		twi_stop();

		if (status == 0)
		{
			current_state = ACK_STATE;
		}
		else
		{
			addr++;
			current_state = IDLE_STATE;
		}
		break;

		// Slave ACK received
		case ACK_STATE:
		uart_puts(" OK\r\n");
		addr++;
		current_state = IDLE_STATE;
		break;
		
		
		default:
		current_state = IDLE_STATE;
	}
}
*/
