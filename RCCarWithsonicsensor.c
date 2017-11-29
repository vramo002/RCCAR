#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "bit.h"
#include "io.c"
#include "scheduler.h"
#include "timer.h"
#include "usart_ATmega1284.h"

#include <util/delay.h>
#include <string.h>
#include <stdlib.h>


#define  Trigger_pin	PA0

int TimerOverflow = 0;

ISR(TIMER3_OVF_vect)
{
	TimerOverflow++;	/* Increment Timer Overflow count */
}

unsigned char variable = 0;
unsigned char modeselect = 0;
double distance;

enum getdata {wait, gettingdata};

int datatick(int state){
	switch(state){
		case wait:
			state = gettingdata;
			break;
		case gettingdata:
			state = wait;
			break;
		default:
			state = wait;
			break;
	}
	switch(state){
		case wait:
			break;
		case gettingdata:
			variable = USART_Receive(0);
			modeselect = variable;
			break;
		default:
			break;
	}
	return state;
}

enum usingdata {wait1, F, B, L, R, G, I, H, J};

int usingtick(int state){
	switch(state){
		case wait1:
			if(variable == 'F' && distance > 10.00){
				state = F;
			}
			else if(variable == 'B'){
				state = B;
			}
			else if(variable == 'L'){
				state = L;
			}
			else if(variable == 'R'){
				state = R;
			}
			else if(variable == 'G'){
				state = G;
			}
			else if(variable == 'I'){
				state = I;
			}
			else if(variable == 'H'){
				state = H;
			}
			else if(variable == 'J'){
				state = J;
			}
			else
				state = wait1;
			break;
		case F:
			if(variable == 'F' && distance > 10.00){
				state = F;
			}
			else
				state = wait1;
			break;
		case B:
			if(variable == 'B'){
				state = B;
			}
			else
				state = wait1;
			break;
		case L:
			if(variable == 'L'){
				state = L;
			}
			else
				state = wait1;
			break;
		case R:
			if(variable == 'R'){
				state = R;
			}
			else
				state = wait1;
			break;
		case G:
			if(variable == 'G'){
				state = G;
			}
			else
				state = wait1;
			break;
		case I:
			if(variable == 'I'){
				state = I;
			}
			else
				state = wait1;
			break;
		case H:
			if(variable == 'H'){
				state = H;
			}
			else
				state = wait1;
			break;
		case J:
			if(variable == 'J'){
				state = J;
			}
			else
				state = wait1;
			break;
		default:
			state = wait1;
		break;
	}
	switch(state){
		case wait1:
			PORTC = 0x00;
			//PORTA = 0x01;
			break;
		case F:
			//PORTA = 0x00;
			PORTC = 0x0A;
			break;
		case B:
			//PORTA = 0x00;
			PORTC = 0x05;
			break;
		case L:
			//PORTA = 0x00;
			PORTC = 0x08;
			break;
		case R:
			//PORTA = 0x00;
			PORTC = 0x02;
			break;
		case G:
			//PORTA = 0x00;
			PORTC = 0x08;
			break;
		case I:
			//PORTA = 0x00;
			PORTC = 0x02;
			break;
		case H:
			//PORTA = 0x00;
			PORTC = 0x04;
			break;
		case J:
			//PORTA = 0x00;
			PORTC = 0x01;
			break;
		default:
			break;
	}
	return state;
}

int main(void)
{
	//DDRA = 0xFF;  PORTA = 0x00; //OUTPUTS
	DDRC = 0xFF;  PORTC = 0x00;
	//DDRB = 0x00;  PORTB = 0xFF; //INPUTS
	
	//Period for the tasks
	unsigned long int SMtest1_calc = 25;
	unsigned long int SMtest2_calc = 50;
	
	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMtest1_calc, SMtest2_calc);
	
	///Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMtest1_period = SMtest1_calc/GCD;
	unsigned long int SMtest2_period = SMtest2_calc/GCD;
	
	//Declare an array of tasks
	static task task1, task2;
	task *tasks[] = { &task1, &task2};
	
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = SMtest1_period;//Task Period.
	task1.elapsedTime = SMtest1_period;//Task current elapsed time.
	task1.TickFct = &datatick;//Function pointer for the tick.
	
	// Task 2
	task2.state = -1;//Task initial state.
	task2.period = SMtest2_period;//Task Period.
	task2.elapsedTime = SMtest2_period;//Task current elapsed time.
	task2.TickFct = &usingtick;//Function pointer for the tick.
	
	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();

	unsigned short i; // Scheduler for-loop iterator
	
	
	
	initUSART(0);
	
	long count;
	DDRA = 0x01;		/* Make trigger pin as output */
	PORTB = 0xFF;
	sei();			/* Enable global interrupt */
	TIMSK3 = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
	TCCR3A = 0;		/* Set all bit to zero Normal operation */
	
	while (1)
	{
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
		
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
}
