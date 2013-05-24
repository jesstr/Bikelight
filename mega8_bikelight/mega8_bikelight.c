/*
 * mega8_bikelight.c
 *
 * Created: 21.07.2012 17:48:27
 * Author: Pavel Cherstvov
 */ 


#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "spi.h"
#include "nRF24L01.h"

#define PWR_PORT	PORTD
#define PWR_DDR		DDRD
#define PWR_PIN		PD6	

#define SWITCH_PORT	PORTD
#define SWITCH_DDR	DDRD
#define SWITCH_PIN	PD7

#define TURN_ON		PWR_PORT|=(1<<PWR_PIN)
#define TURN_OFF	PWR_PORT&=~(1<<PWR_PIN)

#define SWITCH_MODE		SWITCH_DDR|=(1<<SWITCH_PIN);   \
				_delay_ms(10); \
				SWITCH_DDR&=~(1<<SWITCH_PIN); \
				_delay_ms(10)
						
#define POWER_DOWN		MCUCR|=(1<<SE)|(1<<SM1)


#define RX_PAYLOAD_LENGTH 3		/* Fixed data packet length in bytes */ 


volatile unsigned char pwr_on; 		/* pwr_on = 1 - device is switched on
					   pwr_on = 0 - device is switched off */

volatile unsigned char new_rx_data=0;	/* New RX data flag */
 

	 

/* IO start initialization */
void IO_Init(void)
{
	PWR_DDR|=(1<<PWR_PIN);	
	PORTD|=(1<<PD2);	// Ïîäòÿæêà äëÿ INT0
}

/* Interrupts start initialization */
void IRQ_Init(void)
{
	MCUCR|=(1<<ISC01); 		/* INT0 falling edge */
	GICR|=(1<<INT0)|(1<<INT1);	/* INT0 & INT1 enabled */
}

/* Switch blinking mode (instant on, fast blink, slow blink, etc. ) */
void SwitchMode(unsigned char mode)
{
	unsigned char i;
	for (i=0; i<mode; i++) {
		SWITCH_MODE;	
	}
}

/* INT0 interrupt handle, connected to "ON/MODE/OFF" button */ 
ISR(INT0_vect)
{
	//cli();
	if (pwr_on) {
		TURN_OFF;
		pwr_on=0;
	}	
	else {	
		TURN_ON;
		SwitchMode(1);
		pwr_on=1;
	}	
	_delay_ms(250);
	//sei();
}	

/* INT1 interrupt handle, connected to the nRF24L01 "IRQ" pin */
ISR(INT1_vect)
{
	unsigned char buff, i=0;
	
	/* reading the status register */
	SPI_CS1_LOW;
	buff=SPI_ReceiveByte_Master(); 
	SPI_CS1_HIGH;
	
	/* Checking interrupt flags in status register */
	switch ((buff)&(0x70)) {
		/* RX_DR - RX data ready */
		case 0x40: {
			CE_LOW;
			
			/* reading RX buffer */
			SPI_CS1_LOW; 
			SPI_SendByte_Master(R_RX_PAYLOAD); 
			while (i<RX_PAYLOAD_LENGTH) { 
				rx_payload[i++]=SPI_ReceiveByte_Master(); 
			}
			SPI_CS1_HIGH;
						
			_delay_us(2);
			
			/* clearing IRQ flag */
			SPI_CS1_LOW; 
			SPI_SendByte_Master(W_REGISTER|STATUS); 
			SPI_SendByte_Master(buff|(0x40)); 
			SPI_CS1_HIGH; 
			
			CE_HIGH;

			new_rx_data=1;

			break;  
		}
		/* TX_DS - data sent */		
		case 0x20: {
			/* clearing IRQ flag */
			SPI_CS1_LOW; 
			SPI_SendByte_Master(W_REGISTER|STATUS); 
			SPI_SendByte_Master(buff|(0x20));
			SPI_CS1_HIGH; 
			
			break;  
		}
		/* Error: MAX count of TX attempts reached */		
		case 0x10:{
			/* clearing IRQ flag */			
			SPI_CS1_LOW; 
			SPI_SendByte_Master(W_REGISTER|STATUS); 
			SPI_SendByte_Master(buff|(0x10)); 
			SPI_CS1_HIGH; 
			
			break;  
		}
	}
}

/* Main routine */
int main(void)
{
	IO_Init();
	IRQ_Init();
	nRF24L01_Init();
	nRF24L01_Standby_1();
	nRF24L01_SetRXPayloadLenght(RX_PAYLOAD_LENGTH);
	SPI_Init_Master();

	sei();
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
					
    	while(1) {
		if (new_rx_data) {
			new_rx_data=0;
			switch (rx_payload[0]) {
				/* 0x01 - turn on the light instantly */
				case 0x01: {
					TURN_ON;
					SwitchMode(1);
					pwr_on=1;
					break;				
				}
				/* 0x02 - turn off the light */
				case 0x02: {
					TURN_OFF;
					pwr_on=0;
					break;
				}			
			}
		}
		sleep_mode();
    	}
}
