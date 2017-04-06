#include <msp430.h> 
#include <wifi.h>
#include <string.h>
//#include <stdlib.h>

/*
 * main.c
 */
int main(void) {
    WifiSetup();
    long timer = 0;

    char appliance = '0';
    char cooling = '0';
    char fan = '0';

    P1DIR |= BIT0;

    while(1){
        WifiLoop();
        if(NewData() == 0){
            if(strncmp(rxData, "AP|", 3) == 0){
                if(rxData[3] == '0'){
                    appliance = rxData[3];
                }
                else if(rxData[3] == '1'){
                    appliance = rxData[3];
                }

                char tmpStr[9] = "AP|0";
                tmpStr[3] = appliance;

                SendData(tmpStr);
            }
        }
        if(appliance == '1'){
            P1OUT = BIT0;
        }
        else{
            P1OUT = !BIT0;
        }
    }
	return 0;
}
