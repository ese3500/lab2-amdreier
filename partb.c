/*
 * partb.c
 *
 * Created: 2/8/2023 3:18:01 PM
 * Author : amdreier
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

void Initialize(void) {
	cli();
	
	// data dir pin 13
	DDRB |= (1 << DDB5);
	
	// data dir pin 9 in
	DDRB &= ~(1 << DDB0);
	
	// output pin 13-9
	PORTB &= ~(1 << PORTB4);
	
	// look for rising edge
	TCCR1B |= (1 << ICES1);
	
	// Clear interrupt flag
	TIFR1 |= (1 < ICF1);
	
	// Enable input capture int
	TIMSK1 |= (1 << ICIE1);
	
	sei();
}

ISR(TIMER1_CAPT_vect) {
	// toggle LED and edge detection
	TCCR1B ^= (1 << ICES1);
	PORTB ^= (1 << PORTB5);
	
}

int main(void)
{
    Initialize();
    while (1) 
    {
    }
}
