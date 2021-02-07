/* Selman Tabet (@selmantabet - https://selman.io/) - UIN 724009859
Assignment 2 - Question 2

This function switches between three states, akin to an FSM.
The first state - All LEDs are turned off.
The second state - LED1 is turned on and flashes.
The third state - LED2 is turned on and flashes.

The state transition takes place through pressing the pushbutton on the board.
This function works fairly reliably due to the high clock speed of the board.
It is effectively running on a high polling rate to the pushbutton,
i.e. Cycle (and instruction) time would very likely be too short compared
to the button state alteration times.

Tested on an EA LPC4088 QuickStart Board and a PC
running Windows 10 Pro x64 19042.789 (20H2).
*/
#include "mbed.h"
#define LPC_SC_PCONP          (*((volatile unsigned long *) 0x400FC0C4))
#define GPIO0_DIR             (*((volatile unsigned long *) 0x20098000))
#define GPIO0_SET             (*((volatile unsigned long *) 0x20098018))
#define GPIO0_CLEAR           (*((volatile unsigned long *) 0x2009801C))
#define GPIO1_DIR             (*((volatile unsigned long *) 0x20098020))
#define GPIO1_SET             (*((volatile unsigned long *) 0x20098038))
#define GPIO1_CLEAR           (*((volatile unsigned long *) 0x2009803C))

DigitalIn Button(p23); //Initialize pushbutton.
DigitalOut led3(LED3);
DigitalOut led4(LED4);

//Simple software delay
void delay(int time){
    volatile int i;
    for(i=0; i<1500000*time; i++); //Assuming 1.5MHz frequency **READ REPORT**
}

int state0(void){ //Turn off LED1 and LED2.
    GPIO0_SET |= 0x1 << 13;
    GPIO1_SET |= 0x1 << 18;
    while (Button); //Await user press to exit state.
}

int state1(void){ //Turn on LED1.
    GPIO0_SET |= 0x1 << 13; //Turn off LED2.
    while (Button){ //Await user press to exit state.
        GPIO1_SET |= 0x1 << 18;  //Turn LED1 off.
        delay(1);
        GPIO1_CLEAR |= 0x1 << 18; //Turn LED1 on.
        delay(2);
    }
}

int state2(void){ //Turn on LED2
    GPIO1_SET |= 0x1 << 18; //Turn off LED1.
    while (Button){ //Await user press to exit state.
        GPIO0_SET |= 0x1 << 13;  //Turn LED2 off.
        delay(1);
        GPIO0_CLEAR |= 0x1 << 13; //Turn LED2 on.
        delay(2);
    }
}


int main(void){
    //Enable the power/clock on the GPIO peripheral.
    //See page 30 in UM10562 (Setting Bit 15 in the PCONP register).
    LPC_SC_PCONP |= 0x8000;
    //Table 84: LED1 = P1[18]    Table 86: LED2 = P0[13]
    GPIO0_DIR |= 0x1 << 13; //Set the direction of Pin 13 of Port 0 to output.
    GPIO1_DIR |= 0x1 << 18; //Set the direction of Pin 18 of Port 1 to output.
    led3 = 0; led4=0; //Turn off LED3 and LED4.
    Button.mode(PullUp);

    //Start alternating between the three defined states.
    //Button press causes state transition.
    while (1){ //Run forever.
        state0(); //Turn off both LEDs.
        while(!Button); //To avoid skipping states unreliably.
        state1(); //Turn on LED1.
        while(!Button);
        state2(); //Turn off LED1 then turn on LED2.
        while(!Button);
    }
}