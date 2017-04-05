#include <msp430.h> 
#include <wifi.h>
#include <string.h>
#include <stdlib.h>

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

            char tmpStr[20] = "Time: ";
            char tmpNum[10] = "";
            strcat(tmpStr, itoa((int)TimeSinceBoot(), tmpNum, 10));
            strcat(tmpStr, "\r\n");
            SendData(tmpStr);
        }
        if(NewData() == 0){
            SendData(rxData);
        }
    }
	return 0;
}
