#include <msp430.h> 
#include <wifi.h>
#include <string.h>

#define RedLed  BIT0    //%00000001 O port pin position P1.0
#define mDetect BIT4    //%00001000 I port pin position P1.4
#define GreenLed BIT6   //%01000000 O port pin position P1.6


#define IDLE 0
#define SEND_ALARM 1
#define ALARM_SENT 2
#define SEND_CLEAR 3

volatile int state = IDLE;

int main(void) {
    P1DIR |= RedLed + GreenLed;  //1.0, 1.6 as outputs
    P1OUT &= 0x00;               // Shut down everything
    P1DIR &= 0x00;
    P1REN |= BIT4;               // Enable internal pull-up/down resistors
    P1OUT |= BIT4;                   //Select pull-up mode for P1.4
    P1IE |= BIT4;                       // P1.4 interrupt enabled
    P1IES &= ~BIT4;                     // P1.4 rising edge
    P1IFG &= ~BIT4;                  // P1.4 IFG cleared


    WifiSetup();

    while(1)
	{
	      WifiLoop();

	      switch(state){
	          case IDLE:

	          break;
	          case SEND_ALARM:
	              if(SendData("MD|1") == 0){
	                  state = ALARM_SENT;
	              }
	              break;
	          case ALARM_SENT:
	              break;
	          case SEND_CLEAR:
	              if(SendData("MD|0") == 0){
	                  state = IDLE;
	              }
              break;
	      }
	}
	return 0;
}

void __attribute__((interrupt(PORT1_VECTOR))) Port_1(void)
{
   P1OUT ^= BIT0;
   if(state == IDLE){
       state = SEND_ALARM;
       P1IES |= BIT4;
   }
   else if(state == ALARM_SENT){
       state = SEND_CLEAR;
       P1IES &= ~BIT4;
   }

   P1IFG &=~BIT4;                        // P1.3 IFG cleared
}
