
#ifndef _SPI_H_
#define _SPI_H_ 1

//Определение портов апаратного SPI
#define SPI_DDR		DDRB
#define SPI_CS_DDR	DDRB
#define SPI_CS_PORT PORTB

//Определение пинов аппаратного SPI
#define CS1		0
//#define CS2	
//#define CS3	
#define MISO	3
#define MOSI	2
#define SCK		1
 
//Определение портов SPI через UART
#define SPI_UART_DDR		DDRE
#define SPI_UART_CS_DDR		DDRE
#define SPI_UART_CS_PORT	PORTE

//Определение пинов SPI через UART
#define CS1_UART	7
//#define CS2	
//#define CS3	
#define MISO_UART	1
#define MOSI_UART	0
#define SCK_UART	2

#define SPI_CS1_LOW SPI_CS_PORT&=~_BV(CS1);
#define SPI_CS1_HIGH SPI_CS_PORT|=_BV(CS1);
   
// Инициализация аппаратного SPI в режиме Master
void SPI_Init_Master(void);
// Инициализация аппаратного SPI в режиме Slave
void SPI_Init_Slave(void);   
// Отправка байта по SPI в режиме Master
void SPI_SendByte_Master(unsigned char data);  
// Отправка байта по SPI в режиме Slave
void SPI_SendByte_Slave(unsigned char data);  
// Прием байта по SPI в режиме Master
unsigned char SPI_ReceiveByte_Master(void);  
// Прием байта по SPI в режиме Slave
unsigned char SPI_ReceiveByte_Slave(void);
/* Transfer byte from Master */
unsigned char SPI_TransByte_Master(unsigned char);
/* Transfer byte from Slave */
unsigned char SPI_TransByte_Slave(unsigned char);

  
#endif /* _SPI_H */
