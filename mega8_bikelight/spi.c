#include <avr/io.h>
   
   // Инициализация аппаратного SPI в режиме Master
  void SPI_Init_Master(void)
  {
	SPI_DDR |= _BV(CS1) | _BV(SCK) | _BV(MOSI);	
	//SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<CPHA);	/* Enable SPI mode 1, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);			/* Enable SPI mode 0, Master, set clock rate fck/16 */
	SPI_CS1_HIGH;
  }
   
   // Инициализация аппаратного SPI в режиме Slave
  void SPI_Init_Slave(void)
  {
	SPI_DDR = (1<<MISO);
	SPCR = (1<<SPE)|(1<<CPHA); 	/* Enable SPI, Slave */
  }
   
  // Отправка байта по SPI в режиме Master
  void SPI_SendByte_Master(unsigned char data)
  {
		SPDR = data;	//Данные
		while(!(SPSR & (1<<SPIF)))
			; 
  }
  
  // Отправка байта по SPI в режиме Slave
  void SPI_SendByte_Slave(unsigned char data)
  {
		unsigned char temp;
		SPDR = data;	//Данные
		while(!(SPSR & (1<<SPIF)))
			;
		temp=SPDR;
  }
  
  // Прием байта по SPI в режиме Master
  unsigned char SPI_ReceiveByte_Master(void)
  {
		SPDR = 0xff;	//Произвольные данные
		while(!(SPSR & (1<<SPIF)))
			; 
		return SPDR;
  }  
  
  // Прием байта по SPI в режиме Slave
  unsigned char SPI_ReceiveByte_Slave(void)
  {
		while(!(SPSR & (1<<SPIF)))
			; 
		return SPDR;
  }  

/* Transfer byte from Master */
unsigned char SPI_TransByte_Master(unsigned char)
{

}

/* Transfer byte from Slave */
unsigned char SPI_TransByte_Slave(unsigned char)
{

} 

 
