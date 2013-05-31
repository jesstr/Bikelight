//требуется библиотека spi.c, spi.h

#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"
#include "nRF24L01.h"

/* IO pins initialization used to control nRF24L01 module */
void nRF24L01_Init(void)
{
	CE_DDR|=(1<<CE_PIN);
	CE_PORT&=~(1<<CE_PIN);
	IRQ_DDR&=~(1<<IRQ_PIN);		
}

/* Data send routine */
void nRF24L01_SendData(unsigned char *data, unsigned char length)
{
	SPI_CS1_LOW;
	SPI_SendByte_Master(W_TX_PAYLOAD);
	while (length) {
		SPI_SendByte_Master(*data);
		data++;
		length--;		
	}
	SPI_CS1_HIGH;
	CE_PULSE;	
}

/* Setting of RXPayLoad parameter */
void nRF24L01_SetRXPayloadLenght(unsigned char length)
{
	SPI_CS1_LOW;
	SPI_SendByte_Master(W_REGISTER|RX_PW_P0);
	SPI_SendByte_Master(length & 0x1F);
	SPI_CS1_HIGH;	
}

/* Switch receive mode on */
void nRF24L01_Receive_On(void)
{
	unsigned char buff;
	SPI_CS1_LOW;
	SPI_SendByte_Master(R_REGISTER|CONFIG);
	buff=SPI_ReceiveByte_Master();
	SPI_CS1_HIGH;
	_delay_us(2);
	SPI_CS1_LOW;
	SPI_SendByte_Master(W_REGISTER|CONFIG);
	SPI_SendByte_Master(buff|_BV(PRIM_RX));
	SPI_CS1_HIGH;
	CE_HIGH;
	_delay_us(2);		
}

/* Switch receive mode off */
void nRF24L01_Receive_Off(void)
{
	unsigned char buff;
	SPI_CS1_LOW;
	SPI_SendByte_Master(R_REGISTER|CONFIG);
	buff=SPI_ReceiveByte_Master();
	SPI_CS1_HIGH;
	_delay_us(2);
	SPI_CS1_LOW;
	SPI_SendByte_Master(W_REGISTER|CONFIG);
	SPI_SendByte_Master(buff&(~_BV(PRIM_RX)));
	SPI_CS1_HIGH;
	CE_LOW;
	_delay_us(2);		
}

/* Get received data */
void nRF24L01_ReadData(unsigned char *data)
{
	SPI_CS1_LOW;
	
	SPI_CS1_HIGH;		
}

/* Switch to Standby 1 mode */
void nRF24L01_Standby_1(void)
{
	unsigned char buff;
	SPI_CS1_LOW;
	SPI_SendByte_Master(R_REGISTER|CONFIG);
	buff=SPI_ReceiveByte_Master();
	SPI_CS1_HIGH;
	_delay_us(2);
	SPI_CS1_LOW;
	SPI_SendByte_Master(W_REGISTER|CONFIG);
	SPI_SendByte_Master(buff|_BV(PWR_UP));
	SPI_CS1_HIGH;
}

/* Switch to Power Down mode */
void nRF24L01_PowerDown(void)
{
	unsigned char buff;
	SPI_CS1_LOW;
	SPI_SendByte_Master(R_REGISTER|CONFIG);
	buff=SPI_ReceiveByte_Master();
	SPI_CS1_HIGH;
	_delay_us(2);
	SPI_CS1_LOW;
	SPI_SendByte_Master(W_REGISTER|CONFIG);
	SPI_SendByte_Master(buff&(~_BV(PWR_UP)));
	SPI_CS1_HIGH;		
}

/* Get the status of the module */
unsigned char nRF24L01_GetStatus(void)
{
	unsigned char buff;
	SPI_CS1_LOW;
	buff=SPI_ReceiveByte_Master();
	SPI_CS1_HIGH;
	return buff;		
}

/* Read config settings from the module */
unsigned char nRF24L01_GetSConfig(void)
{
	unsigned char buff;
	SPI_CS1_LOW;
	SPI_SendByte_Master(R_REGISTER|CONFIG);
	buff=SPI_ReceiveByte_Master();
	SPI_CS1_HIGH;
	return buff;	 
}
