#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE *16UL))) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"

volatile int overflow = -1; 
volatile int falling_edge = 0;  //checking for rising edge
volatile int rising_edge = 0;  //checking for falling edge
volatile int is_space = 0;  //  checking for inactivity

void Initialise() {
	cli();

	DDRB &= ~(1 << DDB0); // set PB0 as input
	DDRD |= (1 << DDD5);  // set PD5 as output
	PORTB |= (1<< PORTB0);

	//prescaling timer by 256
	TCCR1B &= ~(1 << CS10);
	TCCR1B &= ~(1 << CS11);
	TCCR1B |= (1 << CS12);

	//setting up the timer
	TCCR1A &= ~(1 << WGM10);
	TCCR1A &= ~(1 << WGM11);
	TCCR1B |= (1 << WGM12);
	TCCR1B &= ~(1 << WGM13);

	//look for falling edge
	TCCR1B &= ~(1 << ICES1);

	//clear interrupt
	TIFR1 |= (1 << ICF1);

	
	TIMSK1 |= (1 << ICIE1);
	//TIMSK1 |= (1 << TOIE1);  //timer overflow
	TIMSK1 |= (1 << OCIE1A); //Output compare

	OCR1A = 24999;

	sei();
}

ISR(TIMER1_CAPT_vect) {

	

	if (TCCR1B & (1 << ICES1)) {
		rising_edge = ICR1;


		int duration = (((25000 * overflow) - falling_edge) + rising_edge) / 62.5;

		if (duration > 30 && duration < 200) {

			UART_putstring("DOT\n");
			is_space = 0; //reset is_space

			TCNT1 = 0; // restart the timer

		}


		else if (duration >= 200 && duration <= 400) {

			UART_putstring("DASH\n");
			is_space = 0; //reset is_space

			TCNT1 = 0; // restart the timer

		}
		overflow = -1;
	}

	else {
		falling_edge = ICR1;
		overflow = 0;
	}

	TCCR1B ^= (1 << ICES1);

}

ISR(TIMER1_COMPA_vect) {
	if (!is_space) {
		UART_putstring("SPACE\n");
		is_space = 1;
	}
	else {
		if (overflow != -1) {
			overflow ++;
		}
	}

}

int main() {
	UART_init(BAUD_PRESCALER);
	Initialise();
	while (1) {
	}
}

