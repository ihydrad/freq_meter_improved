/*******************************************************
This program was created by the
CodeWizardAVR V3.12 Advanced
Automatic Program Generator
? Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 05.08.2015
Author  : 
Company : 
Comments: 


Chip type               : ATmega328P
Program type            : Application
AVR Core Clock frequency: 16,000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/

#include <mega328p.h>  
#include <delay.h>
#include <alcd.h>
#include <stdio.h>

#define SBR(port, bit)        port |= (1<<bit)
#define CBR(port, bit)        port &= (~(1<<bit))
#define INV(port, bit)        port ^= (1<<bit)
#define SBRC(port, bit)      ((port & (1<<bit)) == 0)
#define SBRS(port, bit)      ((port & (1<<bit)) != 0)

#define  LIGHT_LCD           PORTB.2

#define START_ADC            SBR(ADCSRA, 6)

/*BUTTONS ADC VALUE****************************
UP                   98
DOWN                 253
RIGHT                0
LEFT                 406
SELECT               638
 */
 
#define UP_PRESS_STATE       0
#define DOWN_PRESS_STATE     1
#define RIGHT_PRESS_STATE    2
#define LEFT_PRESS_STATE     3
#define SELECT_PRESS_STATE   4

#define TAHO_PIN             !PINC.5
#define PERIOD               1000*DIV // ms 
#define DIV                  10      // ( 1000/timer IRQ period )

#define STOP_TAH_STATE       0
#define UP_STATE             1

unsigned int 
adc_data,
cnt;

unsigned char
but_state,
state,
array[16];

void tah(unsigned int period);

/*********************************************************
 >INTERRUPT ROUT          ADC
*********************************************************/
interrupt [ADC_INT] void adc_isr(void)
{
 adc_data = ADCW;   
 
 if(adc_data > 50 && adc_data < 200)  SBR(but_state, UP_PRESS_STATE);
 if(adc_data > 200 && adc_data < 300) SBR(but_state, DOWN_PRESS_STATE);
 if(!adc_data || adc_data < 50)       SBR(but_state, RIGHT_PRESS_STATE);
 if(adc_data > 300 && adc_data < 500) SBR(but_state, LEFT_PRESS_STATE);
 if(adc_data > 500 && adc_data < 700) SBR(but_state, SELECT_PRESS_STATE);    
}

/*********************************************************
 >INTERRUPT ROUT          TIMER 100us
*********************************************************/
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
TCNT0=0x38;

tah(PERIOD);
}

void init_dev()
{
#pragma optsize-
CLKPR=(1<<CLKPCE);
CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port B initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
DDRB=(1<<DDB7) | (1<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
// State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (1<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
DDRD=(1<<DDD7) | (1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3) | (1<<DDD2) | (1<<DDD1) | (1<<DDD0);
// State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 16000,000 kHz
// Mode: Normal top=0xFF
// OC0A output: Disconnected
// OC0B output: Disconnected
// Timer Period: 0,01 ms
TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (0<<WGM01) | (0<<WGM00);
TCCR0B=(0<<WGM02) | (0<<CS02) | (1<<CS01) | (0<<CS00);
TCNT0=0x38;
OCR0A=0x00;
OCR0B=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: 15,625 kHz
// Mode: Normal top=0xFF
// OC2A output: Disconnected
// OC2B output: Disconnected
// Timer Period: 9,984 ms
ASSR=(0<<EXCLK) | (0<<AS2);
TCCR2A=(0<<COM2A1) | (0<<COM2A0) | (0<<COM2B1) | (0<<COM2B0) | (0<<WGM21) | (0<<WGM20);
TCCR2B=(0<<WGM22) | (1<<CS22) | (1<<CS21) | (1<<CS20);
TCNT2=0x64;
OCR2A=0x00;
OCR2B=0x00;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=(0<<OCIE0B) | (0<<OCIE0A) | (1<<TOIE0);

// Timer/Counter 1 Interrupt(s) initialization
TIMSK1=(0<<ICIE1) | (0<<OCIE1B) | (0<<OCIE1A) | (0<<TOIE1);

// Timer/Counter 2 Interrupt(s) initialization
TIMSK2=(0<<OCIE2B) | (0<<OCIE2A) | (0<<TOIE2);

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// Interrupt on any change on pins PCINT0-7: Off
// Interrupt on any change on pins PCINT8-14: Off
// Interrupt on any change on pins PCINT16-23: Off
EICRA=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
EIMSK=(0<<INT1) | (0<<INT0);
PCICR=(0<<PCIE2) | (0<<PCIE1) | (0<<PCIE0);

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: Off
// USART Transmitter: On
// USART0 Mode: Asynchronous
// USART Baud Rate: 9600
UCSR0A=(0<<RXC0) | (0<<TXC0) | (0<<UDRE0) | (0<<FE0) | (0<<DOR0) | (0<<UPE0) | (0<<U2X0) | (0<<MPCM0);
UCSR0B=(0<<RXCIE0) | (0<<TXCIE0) | (0<<UDRIE0) | (0<<RXEN0) | (1<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);
UCSR0C=(0<<UMSEL01) | (0<<UMSEL00) | (0<<UPM01) | (0<<UPM00) | (0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);
UBRR0H=0x00;
UBRR0L=0x67;

// Analog Comparator initialization
// Analog Comparator: Off
// The Analog Comparator's positive input is
// connected to the AIN0 pin
// The Analog Comparator's negative input is
// connected to the AIN1 pin
ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
// Digital input buffer on AIN0: On
// Digital input buffer on AIN1: On
DIDR1=(0<<AIN0D) | (0<<AIN1D);

// ADC initialization
// ADC Clock frequency: 125,000 kHz
// ADC Voltage Reference: AVCC pin
// ADC Auto Trigger Source: ADC Stopped
// Digital input buffers on ADC0: Off, ADC1: On, ADC2: On, ADC3: On
// ADC4: On, ADC5: On
#define ADC_VREF_TYPE ((0<<REFS1) | (1<<REFS0) | (0<<ADLAR))
DIDR0=(0<<ADC5D) | (0<<ADC4D) | (0<<ADC3D) | (0<<ADC2D) | (0<<ADC1D) | (1<<ADC0D);
ADMUX=ADC_VREF_TYPE;
ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
ADCSRB=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

// SPI initialization
// SPI Type: Master
// SPI Clock Rate: 2*4000,000 kHz
// SPI Clock Phase: Cycle Start
// SPI Clock Polarity: Low
// SPI Data Order: MSB First
SPCR=(0<<SPIE) | (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);
SPSR=(1<<SPI2X);

// TWI initialization
// TWI disabled
TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

// Bit-Banged I2C Bus initialization
// I2C Port: PORTC
// I2C SDA bit: 4
// I2C SCL bit: 5
// Bit Rate: 100 kHz
// Note: I2C settings are specified in the
// Project|Configure|C Compiler|Libraries|I2C menu.

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTB Bit 0
// RD - PORTB Bit 7
// EN - PORTB Bit 1
// D4 - PORTD Bit 4
// D5 - PORTD Bit 5
// D6 - PORTD Bit 6
// D7 - PORTD Bit 7
// Characters/line: 16
lcd_init(16);

// Global enable interrupts

}

void but_rout()
{
unsigned char tmp = but_state;

if(SBRS(but_state, UP_PRESS_STATE))
  {
   lcd_clear();
   lcd_puts("UP");
   CBR(but_state, UP_PRESS_STATE);
  } 
  
if(SBRS(but_state, DOWN_PRESS_STATE))
  {
   lcd_clear();
   lcd_puts("DOWN");
   CBR(but_state, DOWN_PRESS_STATE);
  }
  
if(SBRS(but_state, LEFT_PRESS_STATE))
  {
   lcd_clear();
   lcd_puts("LEFT");
   CBR(but_state, LEFT_PRESS_STATE);
  }
  
if(SBRS(but_state, RIGHT_PRESS_STATE))
  {
   lcd_clear();
   lcd_puts("RIGHT");
   CBR(but_state, RIGHT_PRESS_STATE);
  }
  
if(SBRS(but_state, SELECT_PRESS_STATE))
  {
   lcd_clear();
   lcd_puts("SELECT");  
   CBR(but_state, SELECT_PRESS_STATE);
  } 
  
lcd_gotoxy(0, 1);
sprintf(array, "adc=%d, 0x%X", adc_data, tmp);
lcd_puts(array); 
}

void tah(unsigned int period)
{
static unsigned int
tah_cnt;

/*========================================================
                    TAHO ROUT
=========================================================*/
    if(SBRC(state, STOP_TAH_STATE))
    {              
        if(TAHO_PIN) 
            SBR(state, UP_STATE);
        else
        {
            if(SBRS(state, UP_STATE))
            {
                cnt++;
                CBR(state, UP_STATE);
            }     
        }
        
        tah_cnt++; 
        
        if(tah_cnt == period)
        {   
            tah_cnt = 0;
           
            SBR(state, STOP_TAH_STATE);
        }        
    }
}

void main(void)
{
 init_dev();
 #asm("sei")
 LIGHT_LCD = 0;
 lcd_puts("Working..."); 
 delay_ms(1000);
 lcd_clear();
 
while (1)
      {        
       //delay_ms(300); 
       //but_rout();        
       //START_ADC;
        
            if(SBRS(state, STOP_TAH_STATE))
            {   
                lcd_clear();
                lcd_gotoxy(0, 0);
                sprintf(array, "cnt = %d;", cnt);
                cnt = 0;
                lcd_puts(array);
                CBR(state, STOP_TAH_STATE);
            }
      }
}
