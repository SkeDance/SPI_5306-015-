#include <avr/io.h>
#include <avr/interrupt.h>

#define		F_CPU				16000000
#define		BAUD				9600
#define		UBBR_VALUE			((F_CPU / 16 / BAUD) - 1)

#define		DEVICE_ADDRESS		0x01	

#define		TEST_DDR			DDRA
#define		TEST_PORT			PORTA
#define		TEST_LED_UART		1
#define		TEST_LED_SPI		2

#define		SPI_DDR				DDRB
#define		SPI_PORT			PORTB

#define		CHIP_SELECT			4
#define		SCK					1
#define		MOSI				2
#define		MISO				3
	
#define		messageLow			0xCC
#define		messageHigh			0x44

volatile int ms_count = 0;

float temp = 0;
float msg = 0;

void SPI_Init(){

	SPI_DDR = (1 << MOSI) | (1 << SCK) | (1 << CHIP_SELECT);// | (0 << MISO);
	//SPI_PORT = (1 << MOSI) | (1 << SCK) | (1 << CHIP_SELECT) | (1 << MISO);
	
	SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR0);
	SPSR = (1 << SPI2X);
}

void SPI_Transmit_data(uint8_t message){
	//SPI_PORT = (0 << CHIP_SELECT);
	SPDR = message;
	//while(!(SPSR & (1 << SPIF)));
	//SPI_PORT = (1 << CHIP_SELECT);	
}

uint8_t SPI_Recieve_data(uint8_t message){
	uint8_t data;
	//SPI_PORT = (0 << CHIP_SELECT);
	SPDR = message;
	while(!(SPSR & (1 << SPIF)));
	data = SPDR;
	//SPI_PORT = (1 << CHIP_SELECT);
	return data;	
}

void USART_Init( unsigned int ubrr )
{
	{
		/* Настройка скорости передачи */
		UBRR1H = (unsigned char)(ubrr>>8);
		UBRR1L = (unsigned char)ubrr;
		/* Разрешение работы приемника и передатчика */
		UCSR1B = (1<<RXEN)|(1<<TXEN);
		/* Установка формата фрейма: 8 информационных битов, 2 стоповых бита */
		UCSR1C = (1<<USBS)|(3<<UCSZ0);
	}
}

void GPIO_init(){
	TEST_DDR |= (1 << TEST_LED_SPI) | (1 << TEST_LED_UART) ;
	//TEST_PORT |= (1 << TEST_LED_SPI) | (1 << TEST_LED_UART);
	
}

void USART_Transmit(unsigned char data)
{
	while ( !( UCSR1A & (1 << UDRE1)) )
	;
	UDR1 = data;
}

unsigned char USART_Receive(void)
{
	while (!(UCSR1A & (1 << RXC1)));
	return UDR1;
}


void InitTimer0(){
	TIMSK |= (1 << OCIE0);
	TCCR0 = (1 << CS00) | (1 << CS01) | (1 << CS02) | (1 << WGM00) | (1 << WGM01);
	OCR0 = 7;
}

ISR(TIMER0_COMP_vect){
	ms_count++;
}

float convert_Temp(){
	int data[9];
	SPI_PORT = (0 << CHIP_SELECT);
	SPI_Transmit_data(0xCC);
	SPI_PORT = (1 << CHIP_SELECT);
	SPI_PORT = (0 << CHIP_SELECT);
	SPI_Transmit_data(0x44);
	SPI_PORT = (1 << CHIP_SELECT);
	while(ms_count != 100){
		int e;
	}
	ms_count = 0;
	SPI_PORT = (0 << CHIP_SELECT);
	//SPI_Recieve_data(0x00);
	SPI_PORT = (1 << CHIP_SELECT);
	SPI_PORT = (0 << CHIP_SELECT);
	//SPI_Recieve_data(0x00);
	SPI_PORT = (1 << CHIP_SELECT);
	int T_KOD;
	T_KOD = (data[0] | (data[1] << 8));
	temp = (float)T_KOD * 0.0625;
	TEST_PORT ^= (1 << TEST_LED_SPI);
	return temp;
	
}

int main(void)
{
	
	
	InitTimer0();
	SPI_Init();
	USART_Init(UBBR_VALUE);
	GPIO_init();
	
	TEST_PORT ^= (1 << TEST_LED_SPI);
	TEST_PORT ^= (1 << TEST_LED_UART);
	sei();
	
	unsigned char str[12] = "\n\rZaXaRSkAdI";
	unsigned char strlenght = 12;
	unsigned char k = 0;
    while (1) 
    {
		
		msg = convert_Temp();
		//msg = 100.99;
		unsigned char* ptr;
		ptr = (unsigned char*)& msg;
		
		for(int i = 0; i < 4; i++){
			USART_Transmit(*(ptr++));
			if(i == 3){
				while(ms_count != 100){
					int t;
				}
			}
		}
		TEST_PORT ^= (1 << TEST_LED_UART);
		ms_count = 0;
	}
}

