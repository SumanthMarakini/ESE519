/*
 * Continuous_Freq.c
 *
 * Created: 29-Oct-21 8:02:57 PM
 * Author : Sumanth Marakini
 */ 

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE *16UL))) - 1)
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"

#define TRIG PB5 // Trigger pin
#define ECHO PB0 //Echo pin

volatile uint16_t read;
volatile int distance;

volatile int flag;

void initialize(){
	
	cli();
	
	DDRB |= (1<<TRIG);
	DDRB &= ~(1<<ECHO);
	DDRD |= (1<<PD5); // OC0A pin
	
	sei();
}

void buzzer_trigger(){
	TCCR0B = 0x0B;
	TCCR0A = 0x03;
	
	TCCR0A |= (1<<COM0B1);
	
	OCR0A = 239;
	OCR0B = OCR0A/2;
}

void echo_dist(){
	
	read = 0; // timer interrupt capture pin
	
	TCNT1 = 0; // count = 0;
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	
	TCCR1B |= (1<<CS10); //Prescale timer by 64
	TCCR1B |= (1<<CS11);
	TCCR1B &= ~(1<<CS12);
	TCCR1B |= (1<<ICES1); //detecting rising edge
	TIMSK1 |= (1<<ICIE1)|(1<<TOIE1); // enable interrupt capture, overflow interrupts
	
}

ISR (TIMER1_CAPT_vect){
	char text[50];
	if(read){
		if(TCCR1B & (1<<ICES1)){
			TCNT1 = 0;
		}
		else{
			
			distance = (0.034)*ICR1*4;
			sprintf(text,"distance = %d\n",distance);
			UART_putstring(text);
			tone_freq(distance);
		}
		TCCR1B ^= (1<<ICES1);
	}
}

void tone_freq(int distance){
	if(distance < 10){ //starting range
		OCR0A = 119;
		OCR0B = OCR0A/2;
	}
	else if (distance < 20){
		OCR0A = 119;
		OCR0B = OCR0A/2;
	}
	
	else if (distance < 30){
		OCR0A = 127;
		OCR0B = OCR0A/2;
	}
	
	else if (distance < 40){
		OCR0A = 142;
		OCR0B = OCR0A/2;
	}
	
	else if (distance < 50){
		OCR0A = 159;
		OCR0B = OCR0A/2;
	}
	
	else if (distance < 60){
		OCR0A = 179;
		OCR0B = OCR0A/2;
	}
	
	else if (distance < 70){
		OCR0A = 190;
		OCR0B = OCR0A/2;
	}
	
	else if (distance < 80){
		OCR0A = 213;
		OCR0B = OCR0A/2;
	}
	
	else if (distance < 90){ //ending range
		OCR0A = 239;
		OCR0B = OCR0A/2;
	}
	
	else{
		OCR0A = 239;
		OCR0B = OCR0A/2;
	}	
}

ISR (TIMER1_OVF_vect){
	if(read){
		UART_putstring("No Echo");
	}
}

void trigger_call(){
	PORTB |= (1<<TRIG);
	_delay_us(10); //Time to trigger
	PORTB &= ~(1<<TRIG);
	read = 1;
}

int main(void)
{
	UART_init(BAUD_PRESCALER);
	initialize();
	echo_dist();
	buzzer_trigger();
	while (1)
	{
		trigger_call();
		_delay_ms(100);
	}
}
