#include <msp430.h> 
#include <wifi.h>
#include <string.h>

#define RedLed  (0x01u)    //%00000001 O port pin position P1.0
#define fAlarm (0x10u)    //%00001000 I port pin position P1.4
#define GreenLed (0x40u)   //%01000000 O port pin position P1.6

int main(void) {
    WifiSetup();
    P1DIR |= (RedLed+GreenLed); //1.0, 1.6 as outputs
    //P1REN = (fAlarm);          //resister enable for P1.4
    P1OUT &= ~(RedLed+GreenLed); //set to 0
    //P1IE |= (fAlarm);           //interrupt enabled
    //P1IES |=(fAlarm);           //hi/lo edge
    //P1IFG &= ~(fAlarm);         //P1.4 IFG cleared
    //__enable_interrupt();
    while(1)
    {
          WifiLoop();
          if((fAlarm & P1IN)  ==(0x10u))  //Case MDetect ON
          {
                P1OUT |= (RedLed); // Toggle Green LED ON
              //  txData="FA|1"
          }
          else //Case OFF
          {
                P1OUT &= ~ (RedLed); // Toggle Green LED OFF
              //  txData="FA|0"
          }
          //SendData(txData);

    }
    return 0;
}
/*
void __attribute__((interrupt(PORT1_VECTOR))) PORT_1(void)
{
    P1OUT ^=(RedLed);
    P1IFG &= ~ (fAlarm);
   // txData="FA|1";
}
*/
