
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"

volatile int overflow = -1;
volatile int falling_edge = 0; //falling edge
volatile int rising_edge = 0; //rising edge
volatile int is_space = 0;
volatile int pos[5]; //the current position



void Initialise()
{
	cli();
	DDRB |= (1 << DDB1); //red for dot
	DDRB |= (1 << DDB2); //green for dash
	DDRB &= ~(1 << DDB0); //input
	PORTB |= (1 << PORTB0); //enable pullup resistor

	//timer1 prescaled by 256
	TCCR1B &= ~(1 << CS10);
	TCCR1B &= ~(1 << CS11);
	TCCR1B |= (1 << CS12);
	
	//set timer1 to CTC
	TCCR1A &= ~(1 << WGM10);
	TCCR1A &= ~(1 << WGM11);
	TCCR1B |= (1 << WGM12);
	TCCR1B &= ~(1 << WGM13);
	
	//look for falling edge
	TCCR1B &= ~(1 << ICES1);
	
	//clear interrupt flag
	TIFR1 |= (1 << ICF1);
	
	//enable global capture interrupt
	TIMSK1 |= (1 << ICIE1);
	TIMSK1 |= (1 << OCIE1A);
	
	//count up to 25000 - 1
	OCR1A = 24999;

	sei();
}


//clear the current position
void clear_pos() {
	for (int i = 0; i < 6; i++) {
		pos[i] = -1;

	}
}

//Print the letters
void print_pos() {

	if (pos[0] == 0)
	{
		if (pos[1] == -1)
		{
			UART_putstring("E");
		}
		else if (pos[1] == 0)
		{
			if (pos[2] == -1)
			{
				UART_putstring("I");

			}

			else if (pos[2] == 0)
			{
				if (pos[3] == -1)
				{
					UART_putstring("S");
				}
				else if (pos[3] == 0)
				{
					if (pos[4] == -1)
					{
						UART_putstring("H");
					}
					else if (pos[4] == 0)
					{
						UART_putstring("5");
					}
					else if (pos[4] == 1)
					{
						UART_putstring("4");
					}
				}
				else if (pos[3] == 1)
				{
					if (pos[4] == -1)
					{
						UART_putstring("V");
					}

					if (pos[4] == 1)
					{
						UART_putstring("3");
					}
				}
			}

			else if (pos[2] == 1)
			{
				if (pos[3] == -1)
				{
					UART_putstring("U");
				}
				
				if (pos[3] == 0)
				{
					if (pos[4] == -1)
					{
						UART_putstring("F");
					}
				}
				
				if (pos[3] == 1 && pos[4] == 1)
				{
					UART_putstring("2");
				}

			}

		}

		else if (pos[1] == 1)
		{
			if (pos[2] == -1)
			{
				UART_putstring("A");
			}
			
			if (pos[2] == 0)
			{
				if (pos[3] == -1)
				{
					UART_putstring("R");
				}

				if (pos[3] == 0)
				{
					if (pos[4] == -1)
					{
						UART_putstring("L");
					}
				}
			}

			if (pos[2] == 1)
			{
				if (pos[3] == -1)
				{
					UART_putstring("W");
				}
				if (pos[3] == 0 && pos[4] == -1)
				{
					UART_putstring("P");
				}
				if (pos[3] == 1)
				{
					if (pos[4] == -1)
					{
						UART_putstring("J");
					}

					if (pos[4] == 1)
					{
						UART_putstring("1");
					}
				}
			}
		}

	}


	else if (pos[0] == 1)
	{

		if (pos[1] == -1)
		{
			UART_putstring("T");
		}

		else if (pos[1] == 1)
		{
			if (pos[2] == -1)
			{
				UART_putstring("M");
			}
			else if (pos[2] == 1)
			{
				if (pos[3] == -1)
				{
					UART_putstring("O");
				}

				else if (pos[3] == 1)
				{
					if (pos[4] == -1)
					{
						UART_putstring("?");
					}

					else if (pos[4] == 1)
					{
						UART_putstring("0");
					}

					else if (pos[4] == 0)
					{
						UART_putstring("9");
					}


				}
				else if (pos[3] == 0)
				{
					if (pos[4] == -1)
					{
						UART_putstring(".");
					}

					if (pos[4] == 0)
					{
						UART_putstring("8");
					}
				}

			}

			else if (pos[2] == 0)
			{
				if (pos[3] == -1)
				{
					UART_putstring("G");
				}
				
				if (pos[3] == 1)
				{
					if (pos[4] == -1)
					{
						UART_putstring("Q");
					}
				}
				if (pos[3] == 0)
				{
					if (pos[4] == -1)
					{
						UART_putstring("Z");
					}

					if (pos[4] == 0)
					{
						UART_putstring("7");
					}
				}
			}
		}

		else if (pos[1] == 0)
		{
			if (pos[2] == -1)
			{
				UART_putstring("N");
			}
			
			if (pos[2] == 1)
			{
				if (pos[3] == -1)
				{
					UART_putstring("K");
				}

				if (pos[3] == 1)
				{
					if (pos[4] == -1)
					{
						UART_putstring("Y");

					}
				}
				
				if (pos[3] == 0)
				{
					if (pos[4] == -1)
					{
						UART_putstring("C");
					}
				}
			}
			if (pos[2] == 0)
			{
				if (pos[3] == -1)
				{
					UART_putstring("D");

				}

				if (pos[3] == 1 && pos[4] == -1)
				{
					UART_putstring("X");
				}
				if (pos[3] == 0)
				{
					if (pos[4] == -1)
					{
						UART_putstring("B");
					}
					if (pos[4] == 0)
					{
						UART_putstring("6");
					}
				}
			}
		}
	}
	clear_pos();
}


void add_to_num(int c)
{

	for (int i = 0; i < 6; i++)
	{
		if (pos[i] == -1)
		{
			pos[i] = c;
			break;
		}
	}
}

//interrupt for button press
ISR(TIMER1_CAPT_vect)
{
	
	if (TCCR1B & (1 << ICES1))
	{
		//if rising edge, button released
		rising_edge = ICR1;
		int duration = (((25000 * overflow) - falling_edge) + rising_edge) / 62.5;

		if (duration > 30 && duration < 200)
		{
			
			PORTB |= (1 << PORTB1); //light LED
			PORTB &= ~(1 << PORTB2);
			add_to_num(0);
			
			is_space = 0; //space enabled again
			TCNT1 = 0;
		}
		else if (duration >= 200 && duration <= 400) {

			
			PORTB &= ~(1 << PORTB1); //light LED
			PORTB |= (1 << PORTB2);
			add_to_num(1);
			
			is_space = 0; //space enabled
			TCNT1 = 0;
		}
		
		overflow = -1;
	}
	else
	{
		//if falling edge, button pressed
		falling_edge = ICR1;
		overflow = 0;
	}
	TCCR1B ^= (1 << ICES1); //toggle
}

ISR(TIMER1_COMPA_vect) {
	
	if (!is_space)
	{
		
		PORTB &= ~(1 << PORTB1);
		PORTB &= ~(1 << PORTB2);
		print_pos();
		is_space = 1;

	}
	else
	{
		if (overflow != -1)
		{
			overflow++;
		}
	}
}


int main(void)
{
	UART_init(BAUD_PRESCALER);
	Initialise();
	clear_pos();
	while (1)
	{
	}
}
