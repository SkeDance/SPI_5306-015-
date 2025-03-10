#include <avr/io.h>

#define		SPI_DDR			DDRB
#define		SPI_PORT		PORTB

#define		CHIP_SELECT		4
#define		SCK				1
#define		MOSI			2
#define		MISO			3

#define		messageLow		0xCC
#define		messageHigh		0x44

void SPI_Init(){
	SPI_DDR = (1 << MOSI) | (1 << SCK) | (1 << CHIP_SELECT) | (0 << MISO);
	SPI_PORT = (1 << MOSI) | (1 << SCK) | (1 << CHIP_SELECT) | (1 << MISO);
	
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
	SPSR = (0 << SPI2X)
}

void SPI_Transmit_data(uint8_t message){
	SPI_PORT = (0 << CHIP_SELECT);
	SPDR = message;
	while(!(SPSR & (1 << SPIF)));
	SPI_PORT = (1 << CHIP_SELECT);	
}

uint8_t SPI_Recieve_data(uint8_t message){
	uint8_t data;
	SPI_PORT = (0 << CHIP_SELECT);
	SPDR = message;
	while(!(SPSR & (1 << SPIF)));
	data = SPDR;
	SPI_PORT = (1 << CHIP_SELECT);
	return data;	
}

int main(void)
{
	
    /* Replace with your application code */
    while (1) 
    {
    }
}

