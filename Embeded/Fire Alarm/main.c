//fire alarm is active low
#include <msp430.h> 
#include <wifi.h>
#include <string.h>

#define IDLE 0
#define DEBOUNCE_ALARM 1
#define SEND_ALARM 2
#define ALARM_SENT 3
#define SEND_CLEAR 4

#define HOLD_DOWN 2500


volatile int state = IDLE;
volatile long timer = 0;

int main(void) {

    P2REN |= BIT3;               // Enable internal pull-up/down resistors
    P2OUT |= BIT3;                   //Select pull-up mode for P1.4
    P2IE |= BIT3;                       // P1.4 interrupt enabled
    P2IES |= BIT3;                     // P1.4 falling edge
    P2IFG &= ~BIT3;                  // P1.4 IFG cleared


    WifiSetup();

    while(1)
    {
          WifiLoop();

          switch(state){
              case IDLE:

              break;
              case DEBOUNCE_ALARM:
                  if(TimeSinceBoot() >= timer){
                      if(!(P2IN & BIT3)){
                          state = SEND_ALARM;
                      }
                      else{
                          state = IDLE;
                      }
                  }
              break;
              case SEND_ALARM:
                  if(SendData("FA|1") == 0){
                      state = ALARM_SENT;
                  }
                  break;
              case ALARM_SENT:
                  /*
                  if(P2IN & BIT3){
                       state = SEND_CLEAR;
                       P2IES |= BIT3;
                  }
                  */
                  break;
              case SEND_CLEAR:
                  if(SendData("FA|0") == 0){
                      state = IDLE;
                  }
              break;
          }
    }
    return 0;
}

void __attribute__((interrupt(PORT2_VECTOR))) Port_2(void)
{
   if(state == IDLE){
       timer = TimeSinceBoot() + HOLD_DOWN;
       state = DEBOUNCE_ALARM;
       P2IES &= ~ BIT3; //set to rising edge trigger
   }
   else if(state == ALARM_SENT){
       state = SEND_CLEAR;
       P2IES |= BIT3; // set to falling edge
   }

   P2IFG &=~BIT3;                        // P1.3 IFG cleared
}

