/*
 * File:   LabFinaPart1.c
 * Author: Rubi Ballesteros
 *
 * Created on April 8, 2015, 12:40 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "p24FJ64GA002.h"
#include "uartComm.h"
#include <stdio.h>

_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & BKBUG_ON & COE_OFF & ICS_PGx1 &
        FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS32768)

_CONFIG2(IESO_OFF & SOSCSEL_SOSC & WUTSEL_LEG & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_OFF &
        IOL1WAY_OFF & I2C1SEL_PRI & POSCMOD_XT)

#define true 1   // define true to use with bool data type.
#define false 0  // define false for bool data type.
//#define bool int // defines the boolean data type.

#define ON 0
#define OFF 1


#define LED4 LATBbits.LATB15 // For LED 4 (Turned on represents the robot reading Black on the left.)
#define LED5 LATBbits.LATB14 // For LED 5 (Turned on represents the robot reading Black on the middle.)
#define LED6 LATBbits.LATB13 // For LED 6 (Turned on represents the robot reading Black on the right.)

volatile int letra = 0;
volatile int contador = 0;
void initLEDs();
//Pins used for the sensors:
//left 23, middle 24, right 25
//Rb 12-14



volatile char receivedChar;

int main(void) {

    initUART(); // Init UART.
    initLEDs();
    LED4 = OFF;

    while (true) {
    
       }

    }

void initLEDs() {
    TRISBbits.TRISB15 = 0;
    TRISBbits.TRISB14 = 0;
    TRISBbits.TRISB13 = 0;
    LED4 = OFF;
    LED5 = OFF;
    LED6 = OFF;
}


void __attribute__((interrupt,auto_psv)) _U2RXInterrupt(void){
    IFS1bits.U2RXIF = 0;
    LED4=ON;
    LED4=OFF;
    receivedChar = U2RXREG;
  
}