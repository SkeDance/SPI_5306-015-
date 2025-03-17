#include <avr/io.h>
#include <avr/interrupt.h>

#define		F_CPU				      16000000
#define		BAUD				      9600
#define		UBBR_VALUE			  ((F_CPU / 16 / BAUD) - 1)

#define		DEVICE_ADDRESS		0x01	

#define		TEST_DDR			    DDRD
#define		TEST_PORT			    PORTD
#define		TEST_LED_UART		    7
#define		TEST_LED_SPI		    6

#define		SPI_DDR				    DDRB
#define		SPI_PORT			    PORTB

#define		CHIP_SELECT			    2
#define		SCK					        5
#define		MOSI				        3
#define		MISO				        4
	
#define		messageLow			  0xCC
#define		messageHigh			  0x44

volatile int ms_count = 0;

float temp = 0;
float msg = 0;

inline static uint8_t transfer(uint8_t data) {
  SPDR = data;
  asm volatile("nop");
  while(!(SPSR & (1 << SPIF))) ; // wait
  return SPDR;
 }
  
void SPI_MasterInit(void)
 {
 /* Set MOSI and SCK output, all others input */
 SPI_DDR = (1<<MOSI)|(1<<SCK);
 /* Enable SPI, Master, set clock rate fck/16 */
 SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
 }

// void SPI_Init(){
// 	SPI_DDR = (1 << MOSI) | (1 << SCK) | (1 << CHIP_SELECT);// | (0 << MISO);
// 	SPI_DDR &= ~(1 << MISO);
//   SPI_PORT = (1 << MOSI) | (1 << SCK) | (1 << MISO) |(1 << CHIP_SELECT);
	
// 	SPCR = 0b11010000;
// }

// void SPI_Transmit_data(uint8_t message){
// 	SPDR = message;
// 	while(!(SPSR & (1 << SPIF)))
// 	;
// }

// uint8_t SPI_Recieve_data(){
// 	uint8_t data;
// 	SPDR = 0x00;
// 	while(!(SPSR & (1 << SPIF)))
// 	;
// 	data = SPDR;
// 	return SPDR;	
// }

void USART_Init( unsigned int ubrr )
{
	{
		/* Настройка скорости передачи */
		UBRR0H = (unsigned char)(ubrr>>8);
		UBRR0L = (unsigned char)ubrr;
		/* Разрешение работы приемника и передатчика */
		UCSR0B = (1<<RXEN0)|(1<<TXEN0);
		/* Установка формата фрейма: 8 информационных битов, 2 стоповых бита */
		UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	}
}

void GPIO_init(){
	TEST_DDR |= (1 << TEST_LED_SPI) | (1 << TEST_LED_UART) ;
}

void USART_Transmit(unsigned char data)
{
	while ( !( UCSR0A & (1 << UDRE0)) )
	;
	UDR0 = data;
}

unsigned char USART_Receive(void)
{
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}


void InitTimer0(){
	TIMSK0 |= (1 << TOIE0); 
  TCCR0B |= (1 << CS00) | (0 << CS01) | (1 << CS02);
	OCR0A = 70;
}

ISR(TIMER0_OVF_vect){
	ms_count++;
}

float convert_Temp(){
	int data[9] = {0};
	SPI_PORT &= ~(1 << CHIP_SELECT);
	transfer(0xCC);
	transfer(0x44);
	SPI_PORT |= (1 << CHIP_SELECT);
	
	while(ms_count != 100)
	;
	
	ms_count = 0;
	SPI_PORT &= ~(1 << CHIP_SELECT);
	transfer(0x00);
	transfer(0x00);
	SPI_PORT |= (1 << CHIP_SELECT);
	int T_KOD;
	T_KOD = (data[0] | (data[1] << 8));
	temp = (float)T_KOD * 0.0625;
	TEST_PORT ^= (1 << TEST_LED_SPI);
	return temp;
	
}

int main(void)
{
	cli();
		
	sei();
	
	GPIO_init();
	InitTimer0();
	SPI_MasterInit();
	USART_Init(UBBR_VALUE);
	
  while (1){

	  msg = convert_Temp();
    while(ms_count != 100){
	    ;
    }

    ms_count = 0;
	  unsigned char* ptr;
	  ptr = (unsigned char*)& msg;
	
	  for(int i = 0; i < 4; i++){
	    USART_Transmit(*(ptr++));
	  }

	  TEST_PORT ^= (1 << TEST_LED_UART);
	}
}

