/*
 * ds3231.h
 *
 * Created: 25.9.2020 17:22:33
 *  Author: Martin Košút
 */ 


#ifndef DS3231_H_
#define DS3231_H_

typedef struct Time_data
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t date;
	uint8_t month;
	uint8_t year;
}T_Time_data;



#endif /* DS3231_H_ */