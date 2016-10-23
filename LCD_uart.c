// port B is for D0-D7
// PC4 is rs
// PC5 is rw
// PC6 is en
// PB7 is busy


#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "inc/hw_lcd.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_ints.h"

///////////For UART Module/////////////
#include "inc/hw_uart.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/uartstdio.c"


#define LOCK_C (*((volatile unsigned long *)0x40006520))
#define CR_C (*((volatile unsigned long *)0x40006524))

#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F (*((volatile unsigned long *)0x40025524))

// have to initialise port B and port E
#define ldata (*((volatile unsigned long *)0x400053FC)) // bits 7-0 of port B

unsigned char string1[]={'P','r','e','s','s',' ','S','w','i','t','c','h',' ','S','W','1'};
unsigned char string2[]={'G','l','o','w',' ','R','e','d',' ','L','e','d'};
unsigned long int ui32Period;

void hardware_init(void);
void lcd_cmd(unsigned char value);
void lcd_data(unsigned char value);
void lcd_ready(void);
void sendString(char myString[]);

void delay(float itime)
{
	 SysCtlDelay(6700000*2*itime);		// creates ~500ms delay - TivaWare fxn

}

void sendString(char myString[])
{
char i;
for(i=0;i<strlen(myString);i++)
	{
	lcd_data(myString[i]);
	}
}

void lcd_cmd(unsigned char value)
{

lcd_ready();
GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0);
delay(0.001);
ldata=value;
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0); //en=0;
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);  //rs=0
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0);  //rw=0;
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 64); //en=1;
delay(0.001);
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0); //en=0;
return;
}

void lcd_data(unsigned char value)
{
lcd_ready();
GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0);
delay(0.001);
ldata=value;
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0); //en=0;
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 16);  //rs=1;
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0);  //rw=0;
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 64); //en=1;
delay(0.001);
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0); //en=0;
return;
}

void lcd_ready()
{
GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0);
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);  //rs=0;
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 32);  //rw=1;
while((GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_7))==1)
{
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0); //en=0;
delay(0.001);
GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 1); //en=1;
}
return;
}

void hardware_init(void)
{
	//Set CPU Clock to 40MHz. 400MHz PLL/2 = 200 DIV 5 = 40MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// Enable UART0 so that we can configure the clock.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	// change this to select the port/pin you are using.
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	// Use the internal 16MHz oscillator as the UART clock source.
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
	// Select the alternate (UART) function for these pins.
	// Initialize the UART for console I/O.
	UARTStdioConfig(0, 115200, 16000000);


	// ADD Tiva-C GPIO setup - enables port, sets pins 1-3 (RGB) pins for output
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	 LOCK_F=0x4C4F434B;
	 CR_F=GPIO_PIN_0|GPIO_PIN_4;


	LOCK_C=0x4C4F434B;
	CR_C=GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;

	//GPIOPinTypeGPIOInput(GPIO_PORTA_BASE,GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3);
	//GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);


    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_6,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    // Turn on the LED
	//GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0, 31);
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, 0x3f);

}


void main()
{

hardware_init();
	UARTprintf("UART_Service Started \n");  //Checking UART Working or Not

GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
delay(0.0001);
lcd_cmd(0x01); //CLEAR DISPLAY
delay(0.0001);
lcd_cmd(0x38); // 8 BIT DATA , 2LINE, 5X7 DOT MATRIX
lcd_cmd(0x0F); // DISPLAY ON, CURSOR ON, NO BLINK
lcd_cmd(0x06);
lcd_cmd(0x01); //CLEAR DISPLAY
sendString("12345");  //Display "12345" 

}
