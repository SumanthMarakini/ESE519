/*
 * ESE519-Lab2-Q2.c
 * Pooling Part B.c
 * Created: 24-09-2021 12:55:17
 * Author : Arjun and Sumanth
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000UL

void Initialize()
{
	DDRB &= ~(1 << DDB0); //PB0 as input
	DDRB |= (1 << DDB5);  //PB5 as output
	
	//timer prescaling by 8
	TCCR1B &= ~(1 << CS10);
	TCCR1B |= (1 << CS11);
	TCCR1B &= ~(1 << CS12);
	
	//setting up the timer
	TCCR1A &= ~(1 << WGM10);
	TCCR1A &= ~(1 << WGM11);
	TCCR1B &= ~(1 << WGM12);
	TCCR1B &= ~(1 << WGM13);
	
	//Clear input capture
	TIFR1 |= (1 << ICF1);
	
	TCCR1B &= ~(1 << ICES1);
	
	sei();
}

int main(void)
{
    Initialize();

    while (1)
    {
		while(!(TIFR1 & (1 << ICF1)));    
		PORTB |= (1 << PORTB5);
		TIFR1 |= (1 << ICF1);
		TCCR1B |= (1 << ICES1);	
	
		while(!(TIFR1 & (1 << ICF1)));
		PORTB &= ~(1 << PORTB5);
		TIFR1 |= (1 << ICF1);
		TCCR1B &= ~(1 << ICES1);
    }
}