/*
 * main.c
 *
 * Created: 2/10/2023 3:18:01 PM
 * Author : amdreier
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#define MS_30 1874
#define MS_60 255
#define MS_200 12499
#define MS_400 24999
#define str_len 25

char String[str_len];
volatile int buttonPressed = 0;
volatile int dot = 0;
volatile int dash = 0;
volatile int space = 0;
volatile int dash_i = 0;
volatile int space_i = 0;
volatile int newChar = 0;
char currByte = 1;

int spaceCount = 0;

volatile int led_i = 0;


void Initialize(void) {
	cli();
	
	// data dir pins 12-9 out
	DDRB |= (1 << DDB4);
	DDRB |= (1 << DDB3);
	DDRB |= (1 << DDB2);
	DDRB |= (1 << DDB1);
	
	// data dir pin 9 in
	DDRB &= ~(1 << DDB0);
	
	// output pins 12-9 LOW
	PORTB &= ~(1 << PORTB4);
	PORTB &= ~(1 << PORTB3);
	PORTB &= ~(1 << PORTB2);
	PORTB &= ~(1 << PORTB1);
	
	
	/* TIMER 1 setup */
	// look for rising edge
	TCCR1B |= (1 << ICES1);
	
	// set Timer1 prescaler clk/256
	TCCR1B |= (1 << CS12);
	
	// Clear interrupt flag
	TIFR1 |= (1 << ICF1);
	
	// Enable input capture int
	TIMSK1 |= (1 << ICIE1);
	
	// Disable compare match A (wait for first button press)
	TIMSK1 &= ~(1 << OCIE1A);
	
	// set for CTC
	TCCR1A &= ~(0b11);
	TCCR1B &= ~(1 << WGM13);
	TCCR1B |= (1 << WGM12);
	
	// Timer for checking space
	OCR1A = MS_400;
	
	/* TIMER 2 setup */
	// set Timer2 prescaler clk/1024
	TCCR2B |= 0b111;
	
	// Disable compare match A (wait for first button press)
	TIMSK2 &= ~(1 << OCIE2A);
	
	// set for CTC
	TCCR2A &= ~(1 << WGM20);
	TCCR2A |= (1 << WGM21);
	TCCR2B &= ~(1 << WGM22);
	
	// Timer for checking space
	OCR2A = MS_60;
	
	UART_init(BAUD_PRESCALER);
	
	sei();
}

// Button press ISR
ISR(TIMER1_CAPT_vect) {
	cli();
	// if button was previously pressed (button now released) compare value for dash/dot if it hasn't already been held for enough time, then restart and enable timer
	if (buttonPressed) {
		unsigned int time = TCNT1;
		if (time >= MS_200 || dash_i) {
			dash = 1;
			dot = 0;
			newChar = 1;
			} else if (time >= MS_30) {
			dot = 1;
			dash = 0;
			newChar = 1;
			} else {
			dash = 0;
			dot = 0;
		}
	} 
	
	space_i = 0;
	dash_i = 0;
	
	TCNT1 = 0;
	
	// Clear interrupt flag
	TIFR1 |= (1 << OCF1A);
	
	// Enable compare match A
	TIMSK1 |= (1 << OCIE1A);
	
	// toggle edge detection
	TCCR1B ^= (1 << ICES1);
	
	// invert button pressed
	buttonPressed = !buttonPressed;
	sei();
}

// State duration ISR
ISR(TIMER1_COMPA_vect) {
	// if button is pressed, it's for dash, else for space
	cli();
	
	
	if (buttonPressed) {
		// Disable compare match A
		TIMSK1 &= ~(1 << OCIE1A);
		dash_i = 1;
		space_i = 0;
	} else {
		space = 1;
		dash_i = 0;
		newChar = 1;
	}
	
	sei();
}

// LED duration ISR
ISR(TIMER2_COMPA_vect) {
	if (led_i < 3) {
		led_i++;
	} else {
		// Disable compare match A turn off LEDs
		TIMSK2 &= ~(1 << OCIE2A);
		PORTB = 0;
		led_i = 0;
	}
}

void blink_dash(void) {
	TCNT2 = 0;
	// Clear interrupt flag
	TIFR2 |= (1 << OCF2A);
	// Enable compare match A
	TIMSK2 |= (1 << OCIE2A);
	PORTB |= (1 << PORTB4);
}

void blink_dot(void) {
	TCNT2 = 0;
	// Clear interrupt flag
	TIFR2 |= (1 << OCF2A);
	// Enable compare match A
	TIMSK2 |= (1 << OCIE2A);
	PORTB |= (1 << PORTB3);
}

char* convert(char input) {
	switch (input) {
		case 0b101:
			return "A";
			break;
		case 0b11000:
			return "B";
			break;
		case 0b11010:
			return "C";
			break;
		case 0b1100:
			return "D";
			break;
		case 0b10:
			return "E";
			break;
		case 0b10010:
			return "F";
			break;
		case 0b1110:
			return "G";
			break;
		case 0b10000:
			return "H";
			break;
		case 0b100:
			return "I";
			break;
		case 0b10111:
			return "J";
			break;
		case 0b1101:
			return "K";
			break;
		case 0b10100:
			return "L";
			break;
		case 0b111:
			return "M";
			break;
		case 0b110:
			return "N";
			break;
		case 0b1111:
			return "O";
			break;
		case 0b10110:
			return "P";
			break;
		case 0b11101:
			return "Q";
			break;
		case 0b1010:
			return "R";
			break;
		case 0b1000:
			return "S";
			break;
		case 0b11:
			return "T";
			break;
		case 0b1001:
			return "U";
			break;
		case 0b10001:
			return "V";
			break;
		case 0b1011:
			return "W";
			break;
		case 0b11001:
			return "X";
			break;
		case 0b11011:
			return "Y";
			break;
		case 0b11100:
			return "Z";
			break;
		case 0b101111:
			return "1";
			break;
		case 0b100111:
			return "2";
			break;
		case 0b100011:
			return "3";
			break;
		case 0b100001:
			return "4";
			break;
		case 0b100000:
			return "5";
			break;
		case 0b110000:
			return "6";
			break;
		case 0b111000:
			return "7";
			break;
		case 0b111100:
			return "8";
			break;
		case 0b111110:
			return "9";
			break;
		case 0b111111:
			return "0";
			break;
		case 0b1:
			if (spaceCount > 1) {
				return "";
				break;
			}
			return " ";
			break;
		default:
			return " INVALID CHAR ";
			break;
			
		
	}
}

void parseByte(void) {
	sprintf(String, convert(currByte));
	UART_putstring(String);
	currByte = 1;
}

int main(void)
{
	Initialize();    
    while (1) 
    {
		if (newChar) {
			if (dash) {
				dash = 0;
				currByte = (currByte << 1) | 0b1;
				blink_dash();
			}
			
			if (dot) {
				dot = 0;
				currByte = (currByte << 1);
				blink_dot();
			}
			
			if (space) {
				if (currByte == 1) {
					spaceCount++;
				} else {
					spaceCount = 0;
				}
				parseByte();
				space = 0;
			}
			
			newChar = 0;
		}		
	}
}
