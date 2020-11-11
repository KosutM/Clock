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
#define btnDOWN				PORTD4
#define btnUP				PORTD5
#define btnOK_MENU			PORTD6
#define btnEXIT				PORTD7


typedef struct  
{
	volatile uint8_t btnDOWN_pressed;
	volatile uint8_t btnUP_pressed;
	volatile uint8_t btnOK_MENU_pressed;
	volatile uint8_t btnEXIT_pressed;
}T_btn_press;
	

T_Time_data time;
T_btn_press btn_press = {
	.btnDOWN_pressed = 0,
	.btnUP_pressed = 0,
	.btnOK_MENU_pressed = 0,
	.btnEXIT_pressed = 0
};
uint8_t status, i, time_done = 1;
uint8_t read_data[10];
uint8_t uart_string[20];
uint8_t hours = 0, minutes = 0, day = 8, month = 10;
uint16_t year = 2020; 

int main(void)
{
	DDRC |= (1<<PORTC5) | (1<<PORTC4);
	DDRD |= (1<<PORTD0) | (1<<PORTD1);
	DDRD &= ~(1<<PORTD2);
	DDRD &= ~((1<<btnDOWN) | (1<<btnUP) | (1<<btnOK_MENU) | (1<<btnEXIT));		//input pins
	PORTD |= (1<<btnDOWN) | (1<<btnUP) | (1<<btnOK_MENU) | (1<<btnEXIT);		//turn on pull up resistors
	MCUCR |= (1<<ISC01);		//falling edge INT0 interrupt
	MCUCR &= ~(1<<ISC00);	
	GICR |= (1<<INT0);			//external interrupt enable
	TCCR0 |= (1<<CS02);			//prescaler 256, 8ms overflow
	TCCR0 &= ~((1<<CS01) | (1<<CS00));	
	TIMSK |= (1<<TOIE0);		//enable timer0 overflow interrupt
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
	twi_init();
	
	sei();
	uart_puts("\r\nstart\r\n");
	_delay_ms(1000);

	DS3231_initialization();
	DS3231_setTime(23,58,5,10,2020);
	
	/*btn_press.btnDOWN_pressed = 0;
	btn_press.btnUP_pressed = 0;
	btn_press.btnOK_MENU_pressed = 0;
	btn_press.btnEXIT_pressed = 0;
	*/
	time_done = 1;
	while (1) 
    {
		//DS3231_getTime(&time);
		if (time_done == 0)
		{
			sprintf(uart_string, "%d.%d.%d %d %d:%d:%d\r\n", time.day, time.month, time.year, time.weekday, time.hours, time.minutes, time.seconds);
			uart_puts(uart_string);
			//uart_puts("\r\n");
			_delay_ms(20);

			time_done = 1;
		}
		/*if (btn_press.btnDOWN_pressed > 10)
		{
			uart_puts("DOWN\r\n");
			_delay_ms(10);
			btn_press.btnDOWN_pressed = 0;
		}
		if (btn_press.btnUP_pressed > 10)
		{
			uart_puts("UP\r\n");
			_delay_ms(10);
			btn_press.btnUP_pressed = 0;
		}*/
		
		if (btn_press.btnOK_MENU_pressed > 50)
		{
			GICR &= ~(1<<INT0);			//external interrupt disable
			uart_puts("MENU_OK\r\n");
			_delay_ms(10);
			while(btn_press.btnEXIT_pressed < 50)
			{
				if (btn_press.btnDOWN_pressed > 10)
				{
					hours--;
					uart_puts("DOWN\r\n");
					_delay_ms(10);
					btn_press.btnDOWN_pressed = 0;
				}
				if (btn_press.btnUP_pressed > 10)
				{
					hours++;
					uart_puts("UP\r\n");
					_delay_ms(10);
					btn_press.btnUP_pressed = 0;
				}
			}
			uart_puts("EXIT\r\n");
			_delay_ms(10);
			DS3231_setTime(hours,minutes,day,month,year);
			GICR |= (1<<INT0);			//external interrupt enable
			//_delay_ms(10);
			btn_press.btnOK_MENU_pressed = 0;
		}
		
		/*if (btn_press.btnEXIT_pressed > 10)
		{
			uart_puts("EXIT\r\n");
			_delay_ms(10);
			btn_press.btnEXIT_pressed = 0;
		}*/
		//_delay_ms(1);
	
    }
}

ISR(INT0_vect)
{
	time_done = 0;
	DS3231_getTime(&time);
}

ISR(TIMER0_OVF_vect)
{
	if ((PIND & (1<<btnDOWN)) == 0)
	{
		btn_press.btnDOWN_pressed++;
	}
	else
	{
		btn_press.btnDOWN_pressed = 0;
	}
	
	if ((PIND & (1<<btnUP)) == 0)
	{
		btn_press.btnUP_pressed++;
	}
	else
	{
		btn_press.btnUP_pressed = 0;
	}
	
	if ((PIND & (1<<btnOK_MENU)) == 0)
	{
		btn_press.btnOK_MENU_pressed++;
	}
	else
	{
		btn_press.btnOK_MENU_pressed = 0;
	}
	
	if ((PIND & (1<<btnEXIT)) == 0)
	{
		btn_press.btnEXIT_pressed++;
	}
	else
	{
		btn_press.btnEXIT_pressed = 0;
	}
	//_delay_us(1);
}
