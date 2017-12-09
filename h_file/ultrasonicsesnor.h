/***************************************************************************************
*    This code was based off the following info
*	 Title: Ultrasonic Module HC-SR04 interfacing with AVR ATmega16/ATmega32
*    Author: ElectronicWings
*    Date: N/A
*    Code version: N/A
*    Availability: http://www.electronicwings.com/avr-atmega/ultrasonic-module-hc-sr04-interfacing-with-atmega1632
*
***************************************************************************************/
#ifndef ULTRASONICSENSOR_H
#define ULTRASONICSENSOR_H

#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

double distance;
long count;
int TimerOverflow = 0;

ISR(TIMER3_OVF_vect)
{
	TimerOverflow++;
}

void init(){
	sei();			
	TIMSK3 = (1 << TOIE3);	
	TCCR3A = 0;		
}

double sensor(){
	PORTA |= (1 << PINA1);
	_delay_us(10);
	PORTA &= (~(1 << PINA1));
		
	TCNT3 = 0;	
	TCCR3B = 0x41;	
	TIFR3 = 1<<ICF3;	
	TIFR3 = 1<<TOV3;	
		
	while ((TIFR3 & (1 << ICF3)) == 0);
	TCNT3 = 0;	
	TCCR3B = 0x01;	
	TIFR3 = 1<<ICF3;	
	TIFR3 = 1<<TOV3;	
	TimerOverflow = 0;
		
	while ((TIFR3 & (1 << ICF3)) == 0);
	count = ICR3 + (65535 * TimerOverflow);	
	distance = (double)count / 466.47;
	return distance;
}

#endif

