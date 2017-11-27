//close to working sonic wierd bug that you have to unlpug something in the curciut to make it work 

//this bug is somehow due to using TIMER3_OVF_vect becasue using TIMER1_OVF_vect works find but that makes no sense since I am using
//TC3 on the microcontroller

//also code is base off of electronicwings.com website
// full url http://www.electronicwings.com/avr-atmega/ultrasonic-module-hc-sr04-interfacing-with-atmega1632 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "io.c"
#include <string.h>
#include <stdlib.h>


#define  Trigger_pin	PA0	

int TimerOverflow = 0;

ISR(TIMER3_OVF_vect)
{
	TimerOverflow++;	/* Increment Timer Overflow count */
}


int main(void)
{
	long count;
	double distance;
	double s = 10.00;

	DDRA = 0x01;		/* Make trigger pin as output */
	PORTB = 0xFF;
	
	DDRC = 0xFF; PORTC = 0x00;
	 
	
	sei();			/* Enable global interrupt */
	TIMSK3 = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
	TCCR3A = 0;		/* Set all bit to zero Normal operation */

    while (1) 
    {
		/* Give 10us trigger pulse on trig. pin to HC-SR04 */
		PORTA |= (1 << Trigger_pin);
		_delay_us(10);
		PORTA &= (~(1 << Trigger_pin));
		
		TCNT3 = 0;	/* Clear Timer counter */
		TCCR3B = 0x41;	/* Capture on rising edge, No prescaler*/
		TIFR3 = 1<<ICF3;	/* Clear ICP flag (Input Capture flag) */
		TIFR3 = 1<<TOV3;	/* Clear Timer Overflow flag */
		
		while ((TIFR3 & (1 << ICF3)) == 0);/* Wait for rising edge */
		TCNT3 = 0;	/* Clear Timer counter */
		TCCR3B = 0x01;	/* Capture on falling edge, No prescaler */
		TIFR3 = 1<<ICF3;	/* Clear ICP flag (Input Capture flag) */
		TIFR3 = 1<<TOV3;	/* Clear Timer Overflow flag */
		TimerOverflow = 0;/* Clear Timer overflow count */
		
		while ((TIFR3 & (1 << ICF3)) == 0);/* Wait for falling edge */
		count = ICR3 + (65535 * TimerOverflow);	/* Take count */
		/* 8MHz Timer freq, sound speed =343 m/s */
		distance = (double)count / 466.47;
		
		if(distance < s){
			PORTC = 0x01;
		}
		else
		{
			PORTC = 0x00;
		}
		
		
		
		_delay_ms(200);
    }
}
