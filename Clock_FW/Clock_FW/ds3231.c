/*
 * ds3231.c
 *
 * Created: 25.9.2020 17:22:18
 *  Author: Martin Košút
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "twi.h"
#include "ds3231.h"

uint8_t i2c_buffer[8];

uint8_t dec2bcd(uint8_t num)		//convert decimal number to binary coded binary
{
	return ((num/10 * 16) + (num % 10));
}

uint8_t bcd2dec(uint8_t num)		//convert binary coded decimal to decimal
{
	return ((num/16 * 10) + (num % 16));
}

uint8_t dayofweek(uint16_t day, uint16_t month, uint16_t year)	//compute day of week (1 Monday - 7 Sunday)
{
	static uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	uint8_t weekday;
	
	year -= month < 3;
	weekday = ( year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;
	weekday = (weekday == 0) ? 7 : weekday;

	return weekday;
}

uint8_t DS3231_initialization()
{
	uint8_t status;
	
	status = twi_start((RTC_address<<1) + TWI_WRITE);		//write control register address
	if (status == 0)
	{
		twi_write(RTC_control_reg);
		twi_stop();
		status = twi_start((RTC_address<<1) + TWI_READ);	//read control register
		if (status == 0)
		{
			i2c_buffer[0] = twi_read_nack();
		}
		else
		{
			twi_stop();
			return 1;
		}
		twi_stop();
		
		i2c_buffer[0] &= ~((1<<2) | (1<<3) | (1<<4));		//RS2, RS1 and INTCN bits to 0 to enable 1Hz square wave output
		
		status = twi_start((RTC_address<<1) + TWI_WRITE);	//write modified control register
		if (status == 0)
		{
			twi_write(RTC_control_reg);
			twi_write(i2c_buffer[0]);
		}
		else
		{
			twi_stop();
			return 1;
		}
		twi_stop();
	}
	else
	{
		return 1;
	}
	return 0;
	
}

uint8_t DS3231_setTime(uint8_t hours, uint8_t minutes, uint8_t day, uint8_t month, uint16_t year)
{
	uint8_t status;
	
	i2c_buffer[0] = 0x00;									//seconds
	i2c_buffer[1] = dec2bcd(minutes);						//minutes
	i2c_buffer[2] = dec2bcd(hours);							//hours
	i2c_buffer[3] = dec2bcd(dayofweek(day, month, year));	//day of week (1 Monday - 7 Sunday)
	i2c_buffer[4] = dec2bcd(day);							//day
	i2c_buffer[5] = dec2bcd(month);							//month	
	i2c_buffer[6] = dec2bcd(year - 2000);					//year
	
	status = twi_start((RTC_address<<1) + TWI_WRITE);		//write data to time registers seconds to years
	if (status == 0)
	{
		twi_write(RTC_seconds_reg);
		for (uint8_t i = 0; i < 7; i++)
		{
			twi_write(i2c_buffer[i]);
		}
		twi_stop();
	}
	else
	{
		twi_stop();
		return 1;											//failed transmission
	}
	return 0;												//successful transmission
}

uint8_t DS3231_getTime(T_Time_data *time)
{
	uint8_t status, i;
	
	status = twi_start((RTC_address<<1) + TWI_WRITE);
	if (status == 0)
	{
		twi_write(RTC_seconds_reg);
		twi_stop();
	}
	else
	{
		twi_stop();
		return 1;
	}
	
	status = twi_start((RTC_address<<1) + TWI_READ);
	if (status == 0)
	{
		for (i = 0; i < 6; i++)
		{
			i2c_buffer[i] = twi_read_ack();
		}
		i2c_buffer[i] = twi_read_nack();
		twi_stop();
	}
	else
	{
		twi_stop();
		return 1;
	}
	
	time->seconds = bcd2dec(i2c_buffer[0]);
	time->minutes = bcd2dec(i2c_buffer[1]);
	time->hours = bcd2dec(i2c_buffer[2]);
	time->weekday = bcd2dec(i2c_buffer[3]);
	time->day = bcd2dec(i2c_buffer[4]);
	time->month = bcd2dec(i2c_buffer[5]);
	time->year = bcd2dec(i2c_buffer[6]);
	
	return 0;
}
