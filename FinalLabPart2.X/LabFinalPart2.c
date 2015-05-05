/*
 * File:   LabFinaPart1.c
 * Author: Rubi Ballesteros
 *
 * Created on April 8, 2015, 12:40 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "p24FJ64GA002.h"
#include "timer.h"
#include "adc.h"
#include "pwm.h"
#include "initSW.h"
#include "uartComm.h"
#include <stdio.h>

_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & BKBUG_ON & COE_OFF & ICS_PGx1 &
        FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS32768)

_CONFIG2(IESO_OFF & SOSCSEL_SOSC & WUTSEL_LEG & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_OFF &
        IOL1WAY_OFF & I2C1SEL_PRI & POSCMOD_XT)

#define true 1   // define true to use with bool data type.
#define false 0  // define false for bool data type.
#define bool int // defines the boolean data type.

#define PRESSED 0 //DOUBLE CHECK THIS it is for switch rb15
#define RELEASED 1 // Define the released state of the switch.

#define black 0
#define white 1

#define ON 0
#define OFF 1


#define LED4 LATBbits.LATB15 // For LED 4 (Turned on represents the robot reading Black on the left.)
#define LED5 LATBbits.LATB14 // For LED 5 (Turned on represents the robot reading Black on the middle.)
#define LED6 LATBbits.LATB13 // For LED 6 (Turned on represents the robot reading Black on the right.)

#define changeThreshold 200 //The threshold at which the black line is detected
#define RightChangeThreshold  200

// DEFINES FOR PART 2.
// TODO: define values for these.
#define BUTTON1 'U' //UP
#define BUTTON2 'D' //DOWN
#define BUTTON3 'C' //CENTER
#define BUTTONLEFT 'L'
#define BUTTONRIGHT 'R'
#define BUTTONSPEED -1 //Increase Speed

void initLEDs();
void turnOnLED(int led);
void assignColors();
//Pins used for the sensors:
//left 23, middle 24, right 25
//Rb 12-14


volatile char dataReceived;
volatile int x;

int main(void) {
    //Initialize components
    initUART(); // Init UART.
    initLEDs();
    initPWMLeft();
    initPWMRight();
    x = 0;
  //  spinForward();
    idleFunction();

    // LED4 = OFF;

    while (true) {
        // spinForward();
        /*
         For part 2, The device will listen for commands with bluetooth while we are in the wait state.
         Other states will be left in, but we will only enter them if the switch is pressed.
         */
        if (x == 1) {
            switch (dataReceived) {
                case BUTTON1:
                    // forward
                    spinForward();
                    x = 0;
                    break;
                case BUTTON2:
                    // backwards
                    spinBackward();
                    x = 0;
                    break;
                case BUTTON3:
                    // stop
                    idleFunction();
                    x = 0;
                    break;
                case BUTTONLEFT:
                    turnLeft();

                    x = 0;
                    break;
                case BUTTONRIGHT:
                    turnRight();
                    x = 0;
                    break;
                default:
                    idleFunction();
                    x = 0;
                    break;
            }
        }
    }
}

//----------------------------------------------------------------------------

void initLEDs() {
    TRISBbits.TRISB15 = 0;
    TRISBbits.TRISB14 = 0;
    TRISBbits.TRISB13 = 0;
    LED4 = OFF;
    LED5 = OFF;
    LED6 = OFF;
}

//-----------------UART Interrupt------------------------------------------------

void __attribute__((interrupt, auto_psv)) _U2RXInterrupt(void) {
    x = 1;
    IFS1bits.U2RXIF = 0;
    LED4 = ON;
    LED4 = OFF;
    dataReceived = U2RXREG;
    LED4 = ON;

}