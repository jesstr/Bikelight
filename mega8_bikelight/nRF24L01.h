
#ifndef _NRF24L01_H_
#define _NRF24L01_H_ 1

/* Begin of defines section to check/edit if switched to another CPU */
#define CE_PIN PB0
#define CE_PORT PORTB
#define CE_DDR DDRB

#define IRQ_PIN PD3
#define IRQ_PORT PORTD
#define IRQ_DDR DDRD
/* End of defines section to check/edit if switched to another CPU */

#define CE_LOW  CE_PORT&=~_BV(CE_PIN)
#define CE_HIGH CE_PORT|=_BV(CE_PIN)

#define CE_PULSE	CE_HIGH; \
			_delay_us(15); \
			CE_LOW

//Список команд
#define R_REGISTER			0b00000000 // + 5 младших бит адреса регистра
#define W_REGISTER			0b00100000 // + 5 младших бит адреса регистра
#define R_RX_PAYLOAD			0b01100001
#define W_TX_PAYLOAD			0b10100000
#define FLUSH_TX			0b11100001
#define FLUSH_RX			0b01100010
#define REUSE_TX_PL			0b11100011
#define ACTIVATE			0b01010000
#define R_RX_PL_WID			0b01100000
#define W_ACK_PAYLOAD			0b10101000 // + 3 младших бита payload (000 - 101)
#define W_TX_PAYLOAD_NO_ACK		0b10110000
#define NOP				0b11111111

//Адреса регистров
#define CONFIG	0x00
#define STATUS	0x07
#define RX_PW_P0	0x11

//Определения некоторых битов
#define PWR_UP 1
#define PRIM_RX 0


/* IO pins initialization used to control nRF24L01 module */
void nRF24L01_Init(void);

/* Data send routine */
void nRF24L01_SendData(unsigned char *data, unsigned char length);

/* Setting of RXPayLoad parameter */
void nRF24L01_SetRXPayloadLenght(unsigned char length);

/* Switch receive mode on */
void nRF24L01_Receive_On(void);

/* Switch receive mode off */
void nRF24L01_Receive_Off(void);

/* Get received data */
void nRF24L01_ReadData(unsigned char *data);

/* Switch to Standby 1 mode */
void nRF24L01_Standby_1(void);

/* Switch to Power Down mode */
void nRF24L01_PowerDown(void);

/* Get the status of the module */
unsigned char nRF24L01_GetStatus(void);

/* Read config settings from the module */
unsigned char nRF24L01_GetSConfig(void);

#endif /* _NRF24L01_H_ */
