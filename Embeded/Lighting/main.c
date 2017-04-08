#include <msp430.h> 
#include <wifi.h>
#include <string.h>
//#include <stdlib.h>

/*
 * main.c
 */

#define IDLE 0
#define DELAY 1
#define TRIGGER 2

volatile int state = 0;
int delayTime = 2;
int onTime = 2;
volatile long timer = 0;
volatile long lastTime = 0;

int main(void) {
    long timer = 0;

    char appliance = '0';

      P1OUT &= 0x00;               // Shut down everything
      P1DIR &= 0x00;
      P1DIR |= BIT0+BIT5;            // P1.0 and P1.5 pins output the rest are input
      P1REN |= BIT4;                   // Enable internal pull-up/down resistors
      P1OUT |= BIT4;                   //Select pull-up mode for P1.4
      P1IE |= BIT4;                       // P1.4 interrupt enabled
      P1IES &= ~BIT4;                     // P1.4 rising edge
      P1IFG &= ~BIT4;                  // P1.4 IFG cleared

      WifiSetup();

    while(1){
        WifiLoop();
//        if(NewData() == 0){
//            if(strncmp(rxData, "LI|", 3) == 0){
//                char tmpStr[9] = "LI|0";
//                tmpStr[3] = appliance;
//                SendData(tmpStr);
//            }
//        }
        switch(state){
            case IDLE:
                break;
            case DELAY:
                if(timer >= TimeSinceBoot() + delayTime){
                    state = TRIGGER;
                    timer = TimeSinceBoot();
                    P1OUT |= BIT5;
                }
                break;
            case TRIGGER:
                if(timer >= TimeSinceBoot() + onTime){
                    state = IDLE;
                    P1OUT &= ~BIT5;
                }
                break;
        }
    }
    return 0;
}

void __attribute__((interrupt(PORT1_VECTOR))) Port_1(void)
{
    lastTime = timer;
    timer = TimeSinceBoot();
    state = DELAY;
    P1OUT &= ~BIT5;

   P1OUT ^= BIT0;
   P1IFG &=~BIT4;                        // P1.3 IFG cleared
}
