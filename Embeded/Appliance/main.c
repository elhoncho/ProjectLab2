#include <msp430.h> 
#include <wifi.h>

/*
 * main.c
 */
int main(void) {
    WifiSetup();
    long timer = 0;
    P1DIR |= BIT0;
    while(1){
        WifiLoop();
        if(TimeSinceBoot() > timer+100){
            timer = TimeSinceBoot();
            P1OUT ^= BIT0;
            SendData("Hello!");
        }
    }
	return 0;
}
