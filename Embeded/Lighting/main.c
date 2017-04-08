#include <msp430.h> 
#include <wifi.h>
#include <string.h>
#include <stdlib.h>

/*
 * main.c
 * The timer is in increments of 1x10^-4 so you have double digit micro second resolution
 */

#define IDLE 0
#define DELAY 1
#define TRIGGER_OFF 2
#define MAX_DELAY 65

volatile int state = 0;
//max of 65
int delayTime = 65;
int onTime = 10;
volatile long triggerTime = 0;
volatile long lastTime = 0;

int main(void) {
    long timer = 0;

      WifiSetup();

      P1OUT &= 0x00;               // Shut down everything
      P1DIR &= 0x00;
      P1DIR |= BIT0+BIT5;            // P1.0 and P1.5 pins output the rest are input
      P1REN |= BIT4;                   // Enable internal pull-up/down resistors
      P1OUT |= BIT4;                   //Select pull-up mode for P1.4
      P1IE |= BIT4;                       // P1.4 interrupt enabled
      P1IES &= ~BIT4;                     // P1.4 rising edge
      P1IFG &= ~BIT4;                  // P1.4 IFG cleared

    while(1){
        WifiLoop();
        if(NewData() == 0){
            if(strncmp(rxData, "LI|", 3) == 0){
                char tmpStr[9] = "";
                int tmpNum = 0;
                strcpy(tmpStr, &rxData[3]);
                tmpNum = atoi(tmpStr);

                if(tmpNum >= MAX_DELAY){
                    delayTime = MAX_DELAY;
                }
                else if(tmpNum <= 0){
                    delayTime = 0;
                }
                else{
                    delayTime = tmpNum;
                }

               SendData(rxData);
            }
        }
        switch(state){
            case IDLE:
                //P1OUT &= ~BIT5;
                break;
            case DELAY:
                if(TimeSinceBoot() >= triggerTime){
                    state = TRIGGER_OFF;
                    lastTime = timer;
                    triggerTime = TimeSinceBoot()+onTime;
                    P1OUT |= BIT5;
                }
                break;
            case TRIGGER_OFF:
                if(TimeSinceBoot() >= triggerTime){
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
    //TODO: Remove this var, it is just for testing
    lastTime = triggerTime;
    triggerTime = TimeSinceBoot() + delayTime;
    state = DELAY;
    P1OUT &= ~BIT5;

   P1OUT ^= BIT0;
   P1IFG &=~BIT4;                        // P1.4 IFG cleared
}
