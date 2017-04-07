#include <msp430.h> 
#include <wifi.h>
#include <string.h>

#define RedLed  (0x01u)    //%00000001 O port pin position P1.0
#define mDetect (0x10u)    //%00001000 I port pin position P1.4
#define GreenLed (0x40u)   //%01000000 O port pin position P1.6

int main(void) {
    WifiSetup();
    P1DIR |= (RedLed+GreenLed); //1.0, 1.6 as outputs
	//P1REN = (mDetect);          //resister enable for P1.4
	P1OUT &= ~(RedLed+GreenLed); //set to 0
	//P1IE |= (mDetect);           //interrupt enabled
	//P1IES |=(mDetect);           //hi/lo edge
	//P1IFG &= ~(mDetect);         //P1.4 IFG cleared
	//__enable_interrupt();
	while(1)
	{
	      WifiLoop();
	      if((mDetect & P1IN)  ==(0x10u))  //Case MDetect ON
	      {
	            P1OUT |= (GreenLed); // Toggle Green LED ON
	          //  txData="MD|1"
	      }
	      else //Case OFF
	      {
	            P1OUT &= ~ (GreenLed); // Toggle Green LED OFF
	          //  txData="MD|0"
	      }
	      //SendData(txData);

	}
	return 0;
}
/*
void __attribute__((interrupt(PORT1_VECTOR))) PORT_1(void)
{
    P1OUT ^=(RedLed);
    P1IFG &= ~ (mDetect);
   // txData="MD|1";
}
*/
