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

    P2DIR |= BIT3;
    P2OUT = 0;

    while(1){
        WifiLoop();
        if(NewData() == 0){

            //Status Update
            if(strncmp(rxData, "ST", 2) == 0){
                char tmpStr[9] = "AP|0";
                tmpStr[3] = appliance;
                SendData(tmpStr);
            }
            else if(strncmp(rxData, "AP|", 3) == 0){
                if(rxData[3] == '0'){
                    appliance = rxData[3];
                    P2OUT &= !BIT3;
                }
                else if(rxData[3] == '1'){
                    appliance = rxData[3];
                    P2OUT |= BIT3;
                }

                char tmpStr[9] = "AP|0";
                tmpStr[3] = appliance;

                SendData(tmpStr);
            }
        }
    }
	return 0;
}
