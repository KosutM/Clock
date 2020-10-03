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
#include "ds3231.h"
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>


#define UART_BAUD_RATE	9600

//#define EEPROM_address	0x57
#define btnDOWN			PORTD4
#define btnUP			PORTD5
#define btnOK_MENU		PORTD6
#define btnEXIT			PORTD7


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


T_Time_data time;
uint8_t status, i;
char read_data[10];
char uart_string[20];

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
	
	
	
	
	_delay_ms(10);
	/*status = twi_start((RTC_address<<1) + TWI_WRITE);
	if (status == 0)
	{
		twi_write(0x0E);
	}
	twi_stop();
	
	status = twi_start((RTC_address<<1) + TWI_READ);
	if (status == 0)
	{
		read_data[0] = twi_read_nack();
	}
	twi_stop();
	*/
	DS3231_initialization();
	DS3231_setTime(19,4,5,10,2020);
	while (1) 
    {
		DS3231_getTime(&time);
		sprintf(uart_string, "%d.%d.%d %d %d:%d:%d\r\n", time.day, time.month, time.year, time.weekday, time.hours, time.minutes, time.seconds);
		
		
		
		uart_puts(uart_string);
		//uart_puts("\r\n");
		//_delay_ms(20);
			/*
			*/
		_delay_ms(1000);
	
    }
}


