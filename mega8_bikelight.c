/*R
 * mega8_bikelight.c
 *
 * Created: 21.07.2012 17:48:27
 *  Author: jesstr
 */ 

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define PWR_PORT	PORTD
#define PWR_DDR		DDRD
#define PWR_PIN		PD6	

#define SWITCH_PORT	PORTD
#define SWITCH_DDR	DDRD
#define SWITCH_PIN	PD7

#define TURN_ON		PWR_PORT|=(1<<PWR_PIN);
#define TURN_OFF	PWR_PORT&=~(1<<PWR_PIN);

#define SWITCH_MODE		SWITCH_DDR|=(1<<SWITCH_PIN);   \
						_delay_ms(10); \
						SWITCH_DDR&=~(1<<SWITCH_PIN); \
						_delay_ms(10);
						
#define POWER_DOWN		MCUCR|=(1<<SE)|(1<<SM1);

volatile unsigned char on;
	 

void IO_Init(void)
{
	PWR_DDR|=(1<<PWR_PIN);
	PORTD|=(1<<PD2);	// Подтяжка для INT0
}

void SwitchMode(unsigned char mode)
{
	unsigned char i;
	for (i=0; i<mode; i++) {
		SWITCH_MODE;	
	}
}

ISR(INT0_vect)
{
	//cli();
	if (on) {
		TURN_OFF;
		on=0;
	}	
	else {	
		TURN_ON;
		SwitchMode(1);
		on=1;
	}	
	_delay_ms(250);
	//sei();
}	

ISR(INT1_vect)
{
	
}	

int main(void)
{
	IO_Init();
	GICR|=(1<<INT0)|(1<<INT1);
	//MCUCR|=(1<<ISC01);
	
	//TURN_ON;
	
	sei();
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//sleep_mode();
					
    while(1) {
		sleep_mode();
    }
}
