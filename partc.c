#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000UL

void Initialize()
{
	cli();
	
	DDRB &= ~(1 << DDB0);  //PB0 as input
	DDRB |= (1 << DDB5);   //PB5 as output
	
	//timer prescaling by 8
	TCCR1B &= ~(1 << CS10);
	TCCR1B |= (1 << CS11);
	TCCR1B &= ~(1 << CS12);
	
	//setting up timer
	TCCR1A &= ~(1 << WGM10);
	TCCR1A &= ~(1 << WGM11);
	TCCR1B &= ~(1 << WGM12);
	TCCR1B &= ~(1 << WGM13);
	
	//Clear input capture 
	TIFR1 |= (1 << ICF1);
	
	// look for falling edge
	TCCR1B &= ~(1 << ICES1);
	
	//enable input capture
	TIMSK1 |= (1 << ICIE1); 
	
	sei();
}

ISR(TIMER1_CAPT_vect)
{
	PORTB ^= (1 << PORTB5);
	TCCR1B ^= (1 << ICES1);
}

int main(void)
{
    Initialize();
	
	while(1)
	{		
	}

}