/*
 * Putting_together.c
 *
 * Created: 29-Oct-21 9:50:45 PM
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

#define TRIG PB4
#define ECHO PB0

volatile uint16_t read;
volatile int distance;
volatile int mode = 0;

void initialize(){
	
	cli();
	
	DDRB |= (1<<TRIG);
	DDRB &= ~(1<<ECHO);
	DDRD |= (1<<PD5); //OC0A pin
	DDRD &= ~(1<<PD7); //using PD7 for mode change
	
	sei();
}

void buzz_trigger(){
	TCCR0B = 0x0B;
	TCCR0A = 0x03;
	
	TCCR0A |= (1<<COM0B1);
	
	OCR0A = 239;
	OCR0B = OCR0A/2;
}

void echo_call(){
	
	read = 0;
	TCNT1 = 0;
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	
	TCCR1B |= (1<<CS10); //Timer Prescale by 64
	TCCR1B |= (1<<CS11);
	TCCR1B &= ~(1<<CS12);
	TCCR1B |= (1<<ICES1); //detecting rising edge
	TIMSK1 |= (1<<ICIE1)|(1<<TOIE1); //Interrupt capture and overflow enabled
	
}

ISR (TIMER1_CAPT_vect){
	char text[50];
	if(read){
		if(TCCR1B & (1<<ICES1)){
			TCNT1 = 0;
		}
		else{
			distance = (0.034)*ICR1*4;
			sprintf(text,"Distance = %d\n",distance);
			UART_putstring(text);
			tone_change(distance);
			
		}
		TCCR1B ^= (1<<ICES1);
	}
}


void tone_change(int distance){
	if(mode==0){
		
		if(distance < 10){ //starting range
			OCR0A = 118;
		}
		else if (distance < 20){
			OCR0A = 118;
		}
	
		else if (distance < 30){
			OCR0A = 128;
		}
	
		else if (distance < 40){
			OCR0A = 141;
		}
	
		else if (distance < 50){
			OCR0A = 159;
		}
	
		else if (distance < 60){
			OCR0A = 180;
		}
	
		else if (distance < 70){
			OCR0A = 189;
		}
	
		else if (distance < 80){
			OCR0A = 213;
		}
	
		else if (distance < 90){
			OCR0A = 238;
		}
	
		else{
			OCR0A = 238;
		}
	}
	else {
		OCR0A = distance * 1.5108 + 102.2396;
	}
}



ISR (TIMER1_OVF_vect){
	if(read){
		UART_putstring("No Echo\n");
	}
}

void trigger_call(){
	PORTB |= (1<<TRIG);
	_delay_us(10);
	PORTB &= ~(1<<TRIG);
	read = 1;
}


volatile int light = 0;
char text[50];
int duty_cycle;

void initialize_adc(){
	
	PRR &= ~(1<<PRADC);
	
	//Select Vref = AVcc
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);

	//Setup ADC Clock div by 128 -> 125KHz
	ADCSRA |= (1<<ADPS0);
	ADCSRA |= (1<<ADPS1);
	ADCSRA |= (1<<ADPS2);

	//Setting Conversion Bit to 0
	ADCSRA &= ~(1<<ADIF);

	//Set to auto trigger
	ADCSRA |= (1<<ADATE);

	//Disable Input Buffer on ADC PIN
	DIDR0 |= (1<<ADC0D);
	
	//Enable ADC
	ADCSRA |= (1<<ADEN);
	//Start Conversion
	ADCSRA |= (1<<ADSC);
	
}
int main(void)
{
    UART_init(BAUD_PRESCALER);
    initialize();
    echo_call();
	buzz_trigger();
	initialize_adc();
    while (1) 
    {
		char change_mode[20];
		trigger_call();
		if(PIND & (1<<PD7)){
			mode = 1; // Continuous mode
		}
		else {
			mode = 0; // discrete mode
		}
		sprintf(change_mode,"Mode value is %d\n",mode);
		UART_putstring(change_mode);
		_delay_ms(100);
		
		while(!(ADCSRA & (1<<ADIF)));
		
		sprintf(text,"ADC value = %d\n",ADC);
		UART_putstring(text);
		
		if (ADC < 190){
			duty_cycle = 0.05;		
		}
		else if(ADC < 280){
			duty_cycle = 0.1;
		}
		else if(ADC < 370){
			duty_cycle = 0.15;
		}
		else if(ADC < 460){
			duty_cycle = 0.20;
		}
		else if(ADC < 550){
			duty_cycle = 0.25;
		}
		else if(ADC < 640){
			duty_cycle = 0.30;
		}
		else if(ADC < 730){
			duty_cycle = 0.35;
		}
		else if(ADC < 820){
			duty_cycle = 0.40;
		}
		else if(ADC < 910){
			duty_cycle = 0.45;
		}
		else {
			duty_cycle = 0.5;
		}
		OCR0B = OCR0A * duty_cycle;
    }
}