/*
 * File:   LabFinaPart1.c
 * Author: Rubi Ballesteros
 *
 * Created on April 8, 2015, 12:40 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "p24FJ64GA002.h"
#include "lcd.h"
#include "timer.h"
#include "adc.h"
#include "pwm.h"
#include "initSW.h"
#include "uartComm.h"
#include <stdio.h>

_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & BKBUG_ON & COE_OFF & ICS_PGx1 &
          FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS32768 )

_CONFIG2( IESO_OFF & SOSCSEL_SOSC & WUTSEL_LEG & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_OFF &
          IOL1WAY_OFF & I2C1SEL_PRI & POSCMOD_XT )

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

//FSM STATES--------------------
typedef enum stateTypeEnum
{
        forwardState,
        turnLeftState,
        turnRightState,
        wait,

} stateType;


//Waiting until the switch is pressed.
volatile stateType currState;

volatile int sensorLeftReading;
volatile int sensorMiddleReading;
volatile int sensorRightReading;

volatile int sensorLeft;
volatile int sensorMiddle;
volatile int sensorRight;

volatile char dataReceived;


int main(void) {
    //Initialize components
    //char v[3];
    initLEDs();
    initPWMLeft();
    initPWMRight();
    initADC();
    initSW1();

    initUART(); // Init UART.

    currState = wait;

    sensorRightReading = rightSensorADC();
    sensorMiddleReading = middleSensorADC();
    sensorLeftReading = leftSensorADC();
    assignColors();
    turnOnLED(13);

    while(true){

        if (currState != wait){ // Don't waste processing power reading the ADC while in wait state.
            sensorRightReading = rightSensorADC();
            sensorMiddleReading = middleSensorADC();
            sensorLeftReading = leftSensorADC();
            assignColors();
            turnOnLED(13);
        }
       switch (currState){
            case wait:
                /*
                 For part 2, The device will listen for commands with bluetooth while we are in the wait state.
                 Other states will be left in, but we will only enter them if the switch is pressed.
                 */

                // TODO: check if waitforChar function works, decide what to do with certain button presses.

                dataReceived = waitForChar(); // Receives an int via UART.

                if (dataReceived == BUTTON1){
                    // forward
                    spinForward();
                }
                if (dataReceived == BUTTON2){
                    // backwards
                    spinBackward();
                }
                if (dataReceived == BUTTON3){
                    // stop
                    idleFunction();
                }
                if (dataReceived == BUTTONLEFT){
                    // Adjust left
                    turnLeft();
                }
                if (dataReceived == BUTTONRIGHT){
                    // Adjust Right
                    turnRight();
                }
                if (dataReceived == BUTTONSPEED){
                    // Adjust Speed
                    ///// Make this function.
                }

                //idleFunction();
                break;
            case forwardState:
                spinForward();
                break;
            case turnRightState:
                turnRight();
                break;
            case turnLeftState:
                turnLeft();
                break;
       }

    }
}

//Interrupt to get out of the Wait state
void _ISR _CNInterrupt(void) {

    IFS1bits.CNIF = 0; //put the flag down

     if(_RB5 == PRESSED){

         if (currState != wait){
             currState = wait;
         }
         else if (currState == wait){
            currState = forwardState;
         }

     }

}

/*
 This interrupts is the one that "listens" the infrared sensors and decides which way to turn
 depending on which sensors are picking up signals.
 */
void _ISR _ADC1Interrupt(void){
    IFS0bits.AD1IF = 0; //Put the interrupt flag down


    //Decide on state based on sensor reading. Right sensor has priority.---------
    if(currState != wait){
        //If the sensors are exactly on the line and outside the line.

        if (sensorLeft == black && sensorMiddle == black && sensorRight == black){
            currState = turnRightState;
        }else if (sensorRight == black){
            currState = turnRightState;
        }else if (sensorLeft == black){
            currState = turnLeftState;
        }else {
            currState = forwardState;
        }

   }
//----------------------------------------------------------------------------

}


void initLEDs(){
	TRISBbits.TRISB15 = 0;
	TRISBbits.TRISB14 = 0;
	TRISBbits.TRISB13 = 0;
	LED4 = OFF;
	LED5 = OFF;
	LED6 = OFF;
}

void turnOnLED(int led){

        if (sensorRight == black){
            LED4 = ON;
        }else{
            LED4 = OFF;
        }

        if (sensorMiddle == black){
            LED5 = ON;
        }else{
            LED5 = OFF;
        }

        if (sensorLeft == black){
            LED6 = ON;
        }else{
            LED6 = OFF;
        }
}

void assignColors(){
    if(sensorRightReading < RightChangeThreshold){ //Buffer of pin 25
        sensorRight = black;
    }else {
        sensorRight = white;
    }

    if(sensorMiddleReading < changeThreshold){ //Buffer of pin 24
        sensorMiddle = black;
    }else {
        sensorMiddle = white;
    }

    if(sensorLeftReading < changeThreshold){ //Buffer of pin 23
        sensorLeft = black;
    }else {
        sensorLeft = white;
    }
}