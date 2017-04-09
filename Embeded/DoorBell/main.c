#include <msp430.h> 
#include <wifi.h>
#include <string.h>

#define IDLE 0
#define RING 1

volatile int state = IDLE;

int main(void) {
    WifiSetup();

    P2OUT &= 0x00;               // Shut down everything
    P2DIR &= 0x00;
    P2OUT |= BIT3;                   //Select pull-up mode for P2.3
    P2IE |= BIT3;                       // P2.3 interrupt enabled
    P2IES &= ~BIT3;                     // P2.3 rising edge
    P2IFG &= ~BIT3;                  // P2.3 IFG cleared

    while(1)
    {
        WifiLoop();

        switch(state){
            case IDLE:
                break;
            case RING:
                if(SendData("DB|1") == 0){
                    //TODO: Its sending multuple DB|1 messages, need to make sure only one is sent
                    state = IDLE;
                }
                break;
        }
    }
    return 0;
}

void __attribute__((interrupt(PORT2_VECTOR))) Port_2(void)
{
   state = RING;
   P2IFG &=~BIT3;                        // P2.3 IFG cleared
}
