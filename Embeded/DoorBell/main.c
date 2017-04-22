#include <msp430.h> 
#include <wifi.h>
#include <string.h>

#define IDLE 0
#define DEBOUNCE 1
#define RING 2

#define HOLD_DOWN 1000

volatile int state = IDLE;
volatile long timer = 0;

int main(void) {
    WifiSetup();

    P2OUT &= 0x00;               // Shut down everything
    P2DIR &= 0x00;
    P2OUT |= BIT3;                   //Select pull-up mode for P2.3
    P2IE |= BIT3;                       // P2.3 interrupt enabled
    P2IES |= BIT3;                     // P2.3 falling edge
    P2IFG &= ~BIT3;                  // P2.3 IFG cleared

    while(1)
    {
        WifiLoop();

        switch(state){
            case IDLE:
                break;
            case DEBOUNCE:
                if(TimeSinceBoot() >= timer){
                    if(!(P2IN & BIT3)){
                        state = RING;
                    }
                    else{
                        state = IDLE;
                    }
                }
                break;
            case RING:
                if(SendData("DB|1") == 0){
                    state = IDLE;
                }
                break;
        }
    }
    return 0;
}

void __attribute__((interrupt(PORT2_VECTOR))) Port_2(void)
{
   timer = TimeSinceBoot() + HOLD_DOWN;
   state = DEBOUNCE;
   P2IFG &=~BIT3;                        // P2.3 IFG cleared
}
