#include <msp430.h> 
#include <wifi.h>
#include <string.h>
#include <stdlib.h>

/*
 * main.c
 */
#define ADC_SAMPLE_DELTA 10000

volatile int rawTmp = 0;
volatile long nextSample = 100000;

int main(void) {
    WifiSetup();
    long timer = 0;

    char heating = '0';
    char cooling = '0';
    char fan = '0';

    int sampleIndex = 0;
    int samples[10] = {0};



    P2DIR |= BIT3+BIT4+BIT5;
    P2OUT = 0;

    P1SEL |= BIT6; //Setup P1.6 for ADC
    ADC10CTL1 = INCH_6 + ADC10DIV_3 ;         // Channel 6, ADC10CLK/3
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON + ADC10IE;  // Vcc & Vss as reference, Sample and hold for 64 Clock cycles, ADC on, ADC interrupt enable
    ADC10AE0 |= BIT6;

    ADC10CTL0 |= ENC + ADC10SC;

    while(1){
        WifiLoop();
        if(NewData() == 0){
            //Status Update
            if(strncmp(rxData, "ST", 2) == 0){
                char tmpStr[9] = "AC|0|0|0";
                tmpStr[3] = heating;
                tmpStr[5] = cooling;
                tmpStr[7] = fan;

                SendData(tmpStr);
            }
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

        if(TimeSinceBoot() >= nextSample){
            if(sampleIndex < 10){
                float tmpVar = rawTmp;
                tmpVar *= 3.32;
                tmpVar -= 500;
                tmpVar /= 10;
                samples[sampleIndex] = (int)tmpVar;
                sampleIndex++;
            }
            else{
                char tmpStr[7] = "TM:";
                char tmpNum[4] = {0};
                int avg = (samples[0] + samples[1] + samples[2] + samples[3] + samples[4] + samples[5] + samples[6] + samples[7] + samples[8] + samples[9])/10;
                itoa(avg,tmpNum,10);
                strcat(tmpStr, tmpNum);
                SendData(tmpStr);
                float tmpVar = rawTmp;
                tmpVar *= 3.55;
                tmpVar -= 500;
                tmpVar /= 10;

                sampleIndex = 0;
                samples[sampleIndex] = (int)tmpVar;
                sampleIndex++;
            }

            ADC10CTL0 |= ENC + ADC10SC;
        }

    }
    return 0;
}

// ADC10 interrupt service routine
void __attribute__((interrupt(ADC10_VECTOR))) ADC10_ISR(void)
{
    rawTmp = ADC10MEM;
    nextSample = TimeSinceBoot() + ADC_SAMPLE_DELTA;
}

