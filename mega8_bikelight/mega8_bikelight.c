/*
 * mega8_bikelight.c
 *
 * Created: 21.07.2012 17:48:27
 * Author: Pavel Cherstvov
 */ 

/*FCU must be defined at "Properties->Toolchain->Symbols" as "F_CPU=16000000". */
/* #define F_CPU 8000000 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "spi.h"
#include "nRF24L01.h"
#include "commands.h"

#define PWR_PORT	PORTD
#define PWR_DDR		DDRD
#define PWR_PIN		PD6	

#define SWITCH_PORT	PORTD
#define SWITCH_DDR	DDRD
#define SWITCH_PIN	PD7

#define TURN_ON		PWR_PORT|=(1<<PWR_PIN); \
					pwr_on=1
#define TURN_OFF	PWR_PORT&=~(1<<PWR_PIN); \
					pwr_on=0

#define SWITCH_MODE		SWITCH_DDR|=(1<<SWITCH_PIN); \
						_delay_ms(10); \
						SWITCH_DDR&=~(1<<SWITCH_PIN); \
						_delay_ms(10)

/* Modes of light */
#define MODE_OFF		TURN_OFF; \
						last_mode = 0			
							
							
#define MODE_INSTANT	TURN_OFF; \
						_delay_ms(10); \
						TURN_ON; \
						last_mode = 1; \
						SwitchMode(last_mode)
										
#define MODE_FASTFLASH	TURN_OFF; \
						_delay_ms(10); \
						TURN_ON; \
						last_mode = 2; \
						SwitchMode(last_mode)
						
#define MODE_SLOWFLASH	TURN_OFF; \
						_delay_ms(10); \
						TURN_ON; \
						last_mode = 3; \
						SwitchMode(last_mode)
																		
#define POWER_DOWN		MCUCR|=(1<<SE)|(1<<SM1)

#define RX_PAYLOAD_LENGTH 3			/* Fixed RX data packet length in bytes */ 
#define TX_PAYLOAD_LENGTH RX_PAYLOAD_LENGTH	/* Fixed TX data packet length in bytes */ 

unsigned char rx_payload[RX_PAYLOAD_LENGTH];
unsigned char tx_payload[TX_PAYLOAD_LENGTH];

volatile unsigned char pwr_on=0; 		/* pwr_on = 1 - device is switched on
					   pwr_on = 0 - device is switched off */

volatile unsigned char last_mode = 0; 		/* Last mode of light which will be restored after on/off */

#define MAX_LIGHT_MODES	3

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
	// MCUCR|=(1<<ISC01); 		/* INT0 falling edge */
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
	/*
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
	*/

		if (last_mode > MAX_LIGHT_MODES) {
			last_mode = 0;
		}
					
		TURN_OFF;
		_delay_ms(10);
		TURN_ON;
		SwitchMode(++last_mode);
		pwr_on=1;

	_delay_ms(250);
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
			
			/* Reading RX buffer */
			SPI_CS1_LOW; 
			SPI_SendByte_Master(R_RX_PAYLOAD); 
			while (i<RX_PAYLOAD_LENGTH) { 
				rx_payload[i++]=SPI_ReceiveByte_Master(); 
			}
			SPI_CS1_HIGH;
						
			_delay_us(2);
			
			/* Clearing IRQ flag */
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
			/* Clearing IRQ flag */
			SPI_CS1_LOW; 
			SPI_SendByte_Master(W_REGISTER|STATUS); 
			SPI_SendByte_Master(buff|(0x20));
			SPI_CS1_HIGH; 
			
			break;  
		}
		/* Error: MAX count of TX attempts reached */		
		case 0x10: {
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
	SPI_Init_Master();
	/*
	TURN_ON;
	SwitchMode(1);
	pwr_on=1;
	*/
	nRF24L01_Init();
	nRF24L01_Standby_1();
	nRF24L01_SetRXPayloadLenght(RX_PAYLOAD_LENGTH);
	nRF24L01_Receive_On();

	sei();
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
					
    	while(1) {
		/* RX commands parsing */
		if (new_rx_data) {
			new_rx_data=0;
			switch (rx_payload[0]) {
				/* COMM_OFF - turn off the light */
				case COMM_OFF: {
					MODE_OFF;
					break;
				}
				/* COMM_ON - turn on the light in previous operating mode */
				case COMM_ON: {
					TURN_ON;
					break;
				}
				/* COMM_INSTANT - switch mode to instantly mode */
				case COMM_INSTANT: {
					MODE_INSTANT;
					break;
				}
				/* COMM_FASTFLASH - switch mode to fast flash mode */
				case COMM_FASTFLASH: {
					MODE_FASTFLASH;
					break;
				}
				/* COMM_SLOWFLASH - switch mode to slow flash mode */
				case COMM_SLOWFLASH: {
					MODE_SLOWFLASH;
					break;
				}			
			}
		}
		sleep_mode();
    	}
}
