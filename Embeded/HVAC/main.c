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

    char heating = '0';
    char cooling = '0';
    char fan = '0';

    P2DIR |= BIT3+BIT4+BIT5;
    P1OUT = 0;

    while(1){
        WifiLoop();
        if(NewData() == 0){
            if(strncmp(rxData, "AC|", 3) == 0){
                if(rxData[3] == '0'){
                    heating = rxData[3];
                    P2OUT &= ~BIT3;
                }
                else if(rxData[3] == '1'){
                    heating = rxData[3];
                    P2OUT |= BIT3;
                }

                if(rxData[5] == '0'){
                    cooling = rxData[5];
                    P2OUT &= ~BIT4;
                }
                else if(rxData[5] == '1'){
                    cooling = rxData[5];
                    P2OUT |= BIT4;
                }

                if(rxData[7] == '0'){
                    fan = rxData[7];
                    P2OUT &= ~BIT5;
                }
                else if(rxData[7] == '1'){
                    fan = rxData[7];
                    P2OUT |= BIT5;
                }

                char tmpStr[9] = "AC|0|0|0";
                tmpStr[3] = heating;
                tmpStr[5] = cooling;
                tmpStr[7] = fan;

                SendData(tmpStr);
            }
        }
//        if(TimeSinceBoot() > timer+100){
//            timer = TimeSinceBoot();
//            P1OUT ^= BIT0;
//
//            char tmpStr[20] = "Time: ";
//            char tmpNum[10] = "";
//            strcat(tmpStr, itoa((int)TimeSinceBoot(), tmpNum, 10));
//            strcat(tmpStr, "\r\n");
//            SendData(tmpStr);
//        }

    }
    return 0;
}
