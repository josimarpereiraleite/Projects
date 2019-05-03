/****************************************************************************

	DISPLAY LCD 4 BITS INTERFACING LIBRARY
	*****************************************
	
	Created:	03/05/2019
	 Author:	JOSIMAR PEREIRA LEITE
	 E-mail:	JOSIMARPEREIRALEITE@gmail.com
	Country:	BRAZIL
	  State:	GOIAS
	   City:	GOIANIA
	   	 			
	 
	******************************************
	
	This file.c is a part of display LCD library for 4 bits interfacing.
	This file is a free software with no warranty. You can use/and/or modify
	
	This library was made to interface my circuit system, but you can modify
	the library as you want. 
	
	This software was based on KS0066 display MCU.
	The communication is made using 8-bits. 
	
	|---------|------------------|
 	|   BITS  |                  |
 	|---------|------------------|
 	|    0    |     ENTIRE       |
 	|---------|------------------|
 	|    1    |     INDECREMENT  |
 	|---------|------------------|
 	|    2    |     BLINK        |
 	|---------|------------------|
 	|    3    |     CURSOR       |
 	|---------|------------------|
 	|    4    |     DISPLAY      |
 	|---------|------------------|
 	|    5    |     FDISPLAY     |
 	|---------|------------------|
 	|    6    |     LINE         |
 	|---------|------------------|
 	|    7    |     DO NOT CARE  |
 	|---------|------------------|
 	
 	
 	Here are some examples of how to use this library to communicate with KS0066.
	First you define the display mode/function you want to use.
	
	KS0066 = (1 << BLINK) | (1 << CURSOR);
	
	After this point, you call the function to initializing the display passing 
	the I/O ports you want to use.
	
	KS0066_INIT( &DDRD, &PORTD, 0xFE );
	
	Well done!
	
	To write on display we can use theses functions:
	
	KS0066_CHR();
	KS0066_STR()  
	KS0066_INT()
	
	KS0066_STR() send a string to display and KS0066_INT() send a interger.

	KS0066_STR("Hello world!");
	
	KS0066_INT(5054);
	
	to change the functionality we can use a simple variable and call the 
	function to accomplish the rest of the task.
	
	if( !(PINB & (1<<0))
	{
		KS0066_DISPLAY_CTR++;
		KS0066_DISPLAY_CONTROLL();
	}
	
	An example how to make the circuit system using AVR-Atmega8 is like this:
	pin PD0 is used to information flag. See test circuit:
	
	DB7		PD7
	DB6		PD6
	DB5		PD5
	DB4		PD4
	EN		PD3
	RW		PD2
	RS		PD1
	FLAG	PD0
	
	______________________________________________________________________
	|                                                                     |
	|                      KS0066 DISPLAY CONTROLLER                      |
	|                                                                     |
	|                                                                     |
	|__VSS__VDD__VEE__RS__RW___E__DB0__DB1__DB2__DB3__DB4__DB5__DB6__DB7__|
 	    |    |    |   |    |   |    |    |    |    |    |    |    |   |
        |	 |    |   |    |   |    |    |    |    |    |    |    |   |
	    |   VCC   |   |    |   |    *----*----*----*    |    |    |   |
	    |         |   |    |   |            |           |    |    |   |
		*---------*   |    |   |           N.C          |    |    |   |
		|             |    |   |                        |    |    |   |
	  -----          PD1  PD2 PD3                      PD4  PD5  PD6  PD7
	   ---
	    -
	
	
	
	
	_________________________________                 |   *Led flag information
	|                             	|               |\| 
	|							  PD0---------------  |-----------	
	|			  A					|               |/|           |
	|			  V				  PD1------RS         |           | 
	|			  R					|                           -----
	|			  				  PD2------RW                    ---
	|			  A					|                             -
	|			  T				  PD3------EN
	|			  M 			  	|
	|			  E				  PD4------DB4	
	|			  G					|
	|			  A	 			  PD5------DB5	
	|			  8					|
	|							  PD6------DB6	
	|								|
	|							  PD7------DB7	
	|								|
	*********************************
	
	
	
	
*******************************************************************************/


#define F_CPU 8000000UL
#include <avr/io.h>
#include "ks0066.h"
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
/*	send a command and character to display
	rs = 0 send a command and rs = 1 send a characters	*/

void KS0066_CMD(unsigned int _rs_pin, unsigned char data)
{
	KS0066_DDRX &=(unsigned char)~(0<<ks0066_RW);		
	KS0066_PORT = (data)|(_rs_pin<<ks0066_RS)|(1<<ks0066_EN);
	KS0066_PORT &=(unsigned char)~(1<<ks0066_EN);
	_delay_ms(1); 
	KS0066_PORT = (data << 4)|(_rs_pin<<ks0066_RS)|(1<<ks0066_EN);
	KS0066_PORT &=(unsigned char)~(1<<ks0066_EN);
	
	//So transfer is performed twice
	//Busy Flag outputs “High” after the second transfer is ended.
	KS0066_BUSYFLAG();
}

/* send a  string to command data, rs = 1	*/
void KS0066_STR(char *String)
{
	while(*String > 0)
		KS0066_CMD(1, *String++);
}

void KS0066_INT(int integer)
{
	char _tmp[50];
	sprintf(_tmp, "%d", integer);
	KS0066_STR(_tmp);	
}

void KS0066_CHR(uint8_t height, uint8_t length, char *String)
{
	if(height) length |=0b01000000;
	length |= 0b10000000;
	KS0066_CMD(0, length);

	while(*String > 0)
		KS0066_CMD(1, *String++);
}

void KS0066_HOME()  { KS0066_CMD(0, 0x02); _delay_ms(1.53); }

void KS0066_CLEAR() { KS0066_CMD(0, 0x01); _delay_ms(1.53); }

void KS0066_SHTR()	{ KS0066_CMD(0, 0x14); _delay_us(37);   }
	
void KS0066_SHTRL() { KS0066_CMD(0, 0x10); _delay_us(37);   }
 
void KS0066_MOVE()  { KS0066_CMD(0, 0x1C); _delay_us(37);   }
	
void KS0066_OFF()   { KS0066_CMD(0, 0x0C); _delay_us(37);   }

void KS0066_DISPLAY_CONTROL()
{
	if (KS0066_DISPLAY_CTR == 1)
	{
		KS0066_CMD(0, 0x0D);
		_delay_ms(40);
	}	
	if (KS0066_DISPLAY_CTR == 2)
	{
		KS0066_CMD(0, 0x0E);
		_delay_ms(40);
	}
	if (KS0066_DISPLAY_CTR == 3)
	{
		KS0066_CMD(0, 0x0C);
		_delay_ms(40);
	}
	if (KS0066_DISPLAY_CTR == 4) KS0066_DISPLAY_CTR = 0;
}

void KS0066_DISPLAY_SHTT();

void KS0066_SPECIAL(unsigned char loc, unsigned char *Characters)
{
	unsigned char i;
    if(loc<8)
    {
     KS0066_CMD(0, 0x40 + (loc*8));      
	 //KS0066_CMD(0, 0x00);                            
       for(i=0; i<8; i++) 
           KS0066_CMD(1, Characters[i]);      
   } 
   
   KS0066_CMD(0, 0x80);
}
/*
void KS0066_BUSYFLAG()
{
	/* BF = “High”, indicates that the internal operation is being processed.
	So during this time the next instruction cannot be accepted. BF can be read through DB7 port
	when RS = “Low” and R/W = “High” (Read Instruction Operation).
	Before executing the next instruction, be sure that BF is not “High”.
	KS0066_PORT = (1<<ks0066_RW)|(0<<ks0066_RS);
	if(!(KS0066_PORT & (1<<BF))){ /* Nothing to do?  PORTD |= (1<<0); _delay_us(0.5); /* 0 us ??  }
	KS0066_PORT = (0<<ks0066_RW)|(0<<ks0066_RS);
	PORTD &=(unsigned char)~(1<<0);
}
*/

void KS0066_BUSYFLAG()
{
	/*NOTE: When an MPU program with checking the Busy Flag(DB7) is made, it must be necessary 1/2Fosc is
	necessary for executing the next instruction by the falling edge of the 'E' signal after the Busy Flag (DB7)
	goes to “Low”.*/
	KS0066_PORT = 0;
	KS0066_PORT = (1 << ks0066_RW)|(1 << ks0066_RS);
	unsigned char flag = KS0066_PORT & (1 << BF);
	do{
		KS0066_PORT = (1 << FLAG);
	} while( flag == 1 );
	KS0066_PORT = (0 << ks0066_RW)|(0 << ks0066_RS);
	// wait 
	_delay_ms(1.53);
}


void KS0066_READ_RAM()
{
	// Read data from	CG or DDRAM
}

void KS0066_WRITE_RAM();
//
//	Receive 8 bits 
//	last bit do not care: Always will be 4 bits interface
//	Bit interface, Line mode, display, display, cursor, blink, decrement or increment, entire shift
//

void KS0066_INIT()
{
	KS0066_DDRX = KS0066_PORT_DIRECTION;
	
	unsigned char _line_mode			= 0;
	unsigned char _f_display			= 0;
	unsigned char _display				= 1;
	unsigned char _cursor				= 0;
	unsigned char _blink				= 0;
	unsigned char _dec_inc				= 0;
	unsigned char _entire				= 0;
	
	if (KS0066	& (1<<6))	_line_mode	= 1;
	if (KS0066	& (1<<5))	_f_display	= 1;
	if (KS0066	& (1<<4))	_display	= 1;
	if (KS0066	& (1<<3))	_cursor		= 1;
	if (KS0066	& (1<<2))	_blink		= 1;
	if (KS0066	& (1<<1))	_dec_inc	= 1;
	if (KS0066	& (1<<0))	_entire		= 1;
	
	unsigned char nf					= 0;
	unsigned char dcb					= 0;
	unsigned char de					= 0;

	
	nf	 = (0xFF & (_line_mode<<3)|(_f_display<<2));
	dcb	 = (0xFF & (1<<3)|(_display<<2)|(_cursor<<1)|(_blink<<0));
	de	 = (0xFF & (1<<2)|(_dec_inc<<1)|(_entire<<0));
	
	//	Wait for more than 30 ms
	_delay_ms(40);
	//	Function set
	KS0066_CMD(0, 2);
	KS0066_CMD(0, 2);
	KS0066_CMD(0, nf);
	//	Wait for more than 39 us
	_delay_us(45);
	//	Display ON/OFF Control
	KS0066_CMD(0, 0);
	KS0066_CMD(0, dcb);
	//	Wait for more than 39 us
	_delay_us(45);
	//	Display Clear
	KS0066_CMD(0, 1);
	//	Wait for more than 1.53 ms
	_delay_ms(1.8);
	//	Entry Mode Set
	KS0066_CMD(0, 0);
	KS0066_CMD(0, de);
	//	Initialization end
}
