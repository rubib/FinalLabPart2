
/*
* File:   uartComm.h
* Author: gv
*
* Created on December 27, 2014, 12:13 PM
*/

// ******************************************************************************************* //
// Defines to simply UART's baud rate generator (BRG) regiser
// given the osicllator freqeuncy and PLLMODE.

#define XTFREQ          7372800         	  // On-board Crystal frequency
#define PLLMODE         4               	  // On-chip PLL setting (Fosc)
#define FCY             (XTFREQ*PLLMODE)/2        // Instruction Cycle Frequency (Fosc/2)

#define BAUDRATE         115200
#define BRGVAL          ((FCY/BAUDRATE)/16)-1
// ******************************************************************************************* //

#define LED4 LATBbits.LATB15
#define ON 0
#define OFF 1

#include "p24fj64ga002.h"
#include "uartComm.h"
#include <stdio.h>

void initUART(){

    //Peripheral Pin Mapping
    RPINR18bits.U1RXR = 9; //pin 18 UART Receive
    RPOR4bits.RP8R = 3; //pin 17 UART Transmit

    //Configuring the UART
    U1BRG  = BRGVAL;
    U1MODE = 0x8000;
    U1MODEbits.PDSEL = 0;// 8 bit no parity
   // U1MODEbits.STSEL = 0; // 1 stop bit
    U1STA  = 0x0440;

    //Putting the UART interrupt flag down.
    IFS0bits.U1RXIF = 0;
}

char waitForChar(){
    char receivedChar;
    // Use the UART RX interrupt flag to wait until we recieve a character.

    receivedChar = U1RXREG;
    LED4 = ON;
//    while(IFS0bits.U1RXIF == 1){
//        // Clear the UART RX interrupt flag to we can detect the reception
//        // of another character.
//        LED4 = ON;
//        IFS0bits.U1RXIF = 0;
//        // U1RXREG stores the last character received by the UART. Read this
//        // value into a local variable before processing.
//        receivedChar = U1RXREG;
//        // Echo the entered character so the user knows what they typed.
//        //printf("%c\n\r", receivedChar);
//    }
    return receivedChar;
}