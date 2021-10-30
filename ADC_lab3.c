/*
 * ADC.c
 *
 * Created: 29-Oct-21 8:29:07 PM
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

volatile int adc_value = 0;
char text[50];

void adc_call(){
	cli();
	
	PRR &= ~(1<<PRADC); // ADC Set
	ADCSRA |= (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2); // Divide clock by 128
	ADMUX = 0x40; //REFS0 = 1, REFS1 = 0, MUXn = 0
	ADCSRA |= (1<<ADATE); //Trigger automatically
	ADCSRB &= ~(1<<ADTS0);
	ADCSRB &= ~(1<<ADTS1);
	ADCSRB &= ~(1<<ADTS2);
	DIDR0 |= (1<<ADC0D); //input buffer disabled
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADSC);
	ADCSRA |= (1<<ADIE);
	
	sei();
}

ISR (ADC_vect){
	adc_value = ADC;
	sprintf(text,"Value of ADC is = %d\n",adc_value);
	UART_putstring(text);
}

int main(void)
{
	UART_init(BAUD_PRESCALER);
	adc_call();
	while (1)
	{
	}
}
