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

int main(void) {
    long timer = 0;

      WifiSetup();

      P2OUT &= 0x00;               // Shut down everything
      P2DIR &= 0x00;
      P2DIR |= BIT0+BIT4;            // P2.4 pins output the rest are input
      P2OUT |= BIT3;                   //Select pull-up mode for P2.3
      P2IE |= BIT3;                       // P2.3 interrupt enabled
      P2IES &= ~BIT3;                     // P2.3 rising edge
      P2IFG &= ~BIT3;                  // P2.3 IFG cleared

    while(1){
        WifiLoop();
        if(NewData() == 0){
            if(strncmp(rxData, "LI|", 3) == 0){
                char tmpStr[9] = "";
                int tmpNum = 0;
                strncpy(tmpStr, &rxData[3], 2);
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

               char tmpNumStr[3] = "00";
               strcpy(tmpStr, "LI|");
               itoa(delayTime, tmpNumStr, 10);
               strcat(tmpStr, tmpNumStr);
               SendData(tmpStr);
            }
        }
        switch(state){
            case IDLE:
                break;
            case DELAY:
                if(TimeSinceBoot() >= triggerTime){
                    state = TRIGGER_OFF;
                    triggerTime = TimeSinceBoot()+onTime;
                    P2OUT |= BIT4;
                }
                break;
            case TRIGGER_OFF:
                if(TimeSinceBoot() >= triggerTime){
                    state = IDLE;
                    P2OUT &= ~BIT4;
                }
                break;
        }
    }
    return 0;
}

void __attribute__((interrupt(PORT2_VECTOR))) Port_2(void)
{
    triggerTime = TimeSinceBoot() + delayTime;
    state = DELAY;
    P2OUT &= ~BIT4;

    P2IFG &=~BIT3;                        // P2.3 IFG cleared
}
