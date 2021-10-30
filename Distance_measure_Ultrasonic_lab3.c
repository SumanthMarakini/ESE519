/*
 * Distance_measure_Ultrasonic.c
 *
 * Created: 29-Oct-21 6:52:27 PM
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


#define TRIG PB4 // Trigger pin
#define ECHO PB0 // Echo pin

volatile uint16_t read;
volatile int distance;

void initialize(){
	
	cli();
	DDRB |= (1<<TRIG);
	DDRB &= ~(1<<ECHO);
	sei();
}

void echo_call(){
	
	read = 0;
	TCNT1 = 0; // count = 0
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	
	TCCR1B |= (1<<CS10); //Prescale timer to 64
	TCCR1B |= (1<<CS11);
	TCCR1B &= ~(1<<CS12);
	
	TCCR1B |= (1<<ICES1); //detecting rising edge
	TIMSK1 |= (1<<ICIE1)|(1<<TOIE1); //interrupt capture, overflow interrupt enabled
	
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
		}
		TCCR1B ^= (1<<ICES1);
	}
}

ISR (TIMER1_OVF_vect){
	if(read){
		UART_putstring("No Echo");
	}
}

void send_trig(){
	PORTB |= (1<<TRIG);
	_delay_us(10); // Time to trigger
	PORTB &= ~(1<<TRIG);
	read = 1;
}

int main(void)
{
	UART_init(BAUD_PRESCALER);
	initialize();
	echo_call();
	while (1)
	{
		send_trig();
		_delay_ms(100);
	}
}