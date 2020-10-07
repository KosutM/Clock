/*
 * ds3231.h
 *
 * Created: 25.9.2020 17:22:33
 *  Author: Martin Košút
 */ 


#ifndef DS3231_H_
#define DS3231_H_

#define RTC_address		0x68

typedef enum RTC_registers
{
	RTC_seconds_reg = 0x00,
	RTC_minutes_reg = 0x01,
	RTC_hours_reg	= 0x02,
	RTC_weekday_reg	= 0x03,
	RTC_date_reg	= 0x04,
	RTC_month_reg	= 0x05,
	RTC_year_reg	= 0x06,
	RTC_control_reg	= 0x0E,
	RTC_status_reg	= 0x0F,
	RTC_aging_reg	= 0x10
}T_RTC_reg;

typedef struct Time_data
{
	volatile uint8_t seconds;
	volatile uint8_t minutes;
	volatile uint8_t hours;
	volatile uint8_t weekday;
	volatile uint8_t day;
	volatile uint8_t month;
	volatile uint8_t year;
}T_Time_data;

//T_Time_data time;

uint8_t dec2bcd(uint8_t num);
uint8_t bcd2dec(uint8_t num);
uint8_t dayofweek(uint16_t day, uint16_t month, uint16_t year);
uint8_t DS3231_initialization();
uint8_t DS3231_setTime(uint8_t hours, uint8_t minutes, uint8_t day, uint8_t month, uint16_t year);
uint8_t DS3231_getTime(T_Time_data *time);


#endif /* DS3231_H_ */