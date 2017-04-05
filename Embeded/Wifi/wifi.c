/*******************************************************************/
/* Things to do                                                    */
/*     Reset if not connected after 30 sec                         */
/*     Need to have a fail safe to break from the while loops      */
/*     Delay reconnects so you dont spam the router                */
/*     Need to add a check to make sure that it is still connected */
/*        incase the CLOSED dosent get read                        */
/*******************************************************************/

#include <msp430.h>
#include <string.h>
#include <stdlib.h>
#include "stringtools.h"
#include "wifi.h"

#define FALSE 0
#define TRUE 1

#define INIT 0
#define RX_DATA 1
#define TX_DATA 2
#define RECONNECT 3

#define TX_SETUP 0
#define TX_VALIDATE 1
#define TX_SEND 2
#define TX_VALIDATE_SEND 3

#define SW_WRITE 0
#define SW_VALIDATE 1

#define RC_CONNECT 0
#define RC_VERIFY 1

//Length of string to ESP, has to have more than the MAX_TX_DATA because of the string AT_CIPSTART="TCP","xxx.xxx.xxx.xxx",xxxxx/r/n and a /0 => 44 chars
#define TX_STRING_LENGTH 44

//Max amount of characters to buffer on the rx
#define RX_STRING_LENGTH 20

//Max amount of characters the user is allowed to send, this must be lower than TX_STRING_LENGTH by 8 characters because of the string +IPD,xx:
#define MAX_TX_DATA 15

struct ringBuffer{
    volatile unsigned int head;
    volatile unsigned int tail;
    volatile char buffer[RX_STRING_LENGTH];
};


static struct ringBuffer inBuffer;
static const char SERVER_IP[] = "10.0.0.6";
static volatile int state = 0;
static volatile int txState = 0;
static volatile char inChar;
static volatile unsigned long msSinceBoot = 0;
static volatile char txData[TX_STRING_LENGTH] = "";


static void SerialWrite(char *TxArray);
static int pushToBuffer(struct ringBuffer *b, const char inChar);
static int popFromBuffer(struct ringBuffer *b, char *outChar);

void WifiSetup(){
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    //Setup the clock
    DCOCTL = 0; // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ; // Set DCO to 16MHz
    DCOCTL = CALDCO_16MHZ; // Set DCO to 16MHz

    //Turn all of port 2 to outputs and set them to low
    P2DIR |= 0XFF;
    P2OUT &= 0X00;

    //Setup timer A
    CCTL0 = CCIE;                             // CCR0 interrupt enabled
    TACTL = TASSEL_2 + MC_1 + ID_3;           // SMCLK/8, upmode
    CCR0 =  20000;                            // 1ms between interrupts

    /* Configure hardware UART */
    P1SEL |= BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
    P1SEL2 |= BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2; // Use SMCLK

    UCA0BR0 = 0x82; // Set baud rate to 9600 with 16MHz clock (Data Sheet 15.3.13)
    UCA0BR1 = 0x06; // Set baud rate to 9600 with 16MHz clock
    UCA0MCTL = UCBRS2+UCBRS1; // Modulation UCBRSx = 6


    UCA0CTL1 &= ~UCSWRST; // Initialize USCI state machine
    IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt

    __bis_SR_register(GIE); //Interrupts enabled

    inBuffer.head = 0;
    inBuffer.tail = 0;
}

void WifiLoop(){

    char outChar;

    static int connected = FALSE;
    static int sendComplete = TRUE;
    static int overflow = FALSE;

    static int reconnectState = 0;

    static char inputString[RX_STRING_LENGTH] = "";
    static char parseBuffer[RX_STRING_LENGTH] = "";
    char txTmpString[TX_STRING_LENGTH] = "";


    ////////////////////////////////////////////////////////////
    //                  Parse the input string                //
    ////////////////////////////////////////////////////////////

    inputString[0] = 0;

    if(popFromBuffer(&inBuffer, &outChar) == 0){
        int len = strlen(parseBuffer);

        if(len < RX_STRING_LENGTH-1){
            parseBuffer[len] = outChar;
            parseBuffer[len+1] = '\0';
        }
        else if(len == RX_STRING_LENGTH-1){
            parseBuffer[0] = parseBuffer[1];
            for(int i = 1; i < RX_STRING_LENGTH-2; i++){
                parseBuffer[i] = parseBuffer[i+1];
            }
            parseBuffer[RX_STRING_LENGTH-2] = outChar;
            parseBuffer[RX_STRING_LENGTH-1] = '\0';
        }


        //TODO: "SEND FAIL" is another posibility if the sending is jacked up some how
        if(endsWith(parseBuffer,"ready")){
            strcpy(inputString, "ready");

            state = 0;
            txState = 0;
            connected = FALSE;
            reconnectState = 0;

            inBuffer.head = 0;
            inBuffer.tail = 0;

            overflow = FALSE;
            sendComplete = TRUE;
        }

        else if(endsWith(parseBuffer,"CLOSED\r\n") && connected){
            strcpy(inputString, "CLOSED");

            state = RECONNECT;
            connected = FALSE;
            reconnectState = 0;
        }
        else if(endsWith(parseBuffer,"CLOSED\r\n")){
            strcpy(inputString, "CLOSED");
        }
        //Careful of the other message "OK"
        else if(endsWith(parseBuffer,"SEND OK\r\n")){
            strcpy(inputString, "SEND OK");
        }
        else if(endsWith(parseBuffer, "OK\r\n")){
            strcpy(inputString, "OK");
        }
        else if(endsWith(parseBuffer,"+IPD,")){
            if(sendComplete && overflow == FALSE){
                strcpy(inputString, "+IPD,");
                state = RX_DATA;
            }
            else{
                overflow = TRUE;
            }
        }
        else if(endsWith(parseBuffer,"busy s...\r\n")){
            strcpy(inputString, "busy s...");
        }
        else if(endsWith(parseBuffer,"WIFI GOT IP\r\n")){
            strcpy(inputString, "WIFI GOT IP");
        }
        else if(endsWith(parseBuffer,"WIFI CONNECTED\r\n")){
            strcpy(inputString, "WIFI CONNECTED");
        }

        else if(endsWith(parseBuffer,"ERROR\r\n")){
            strcpy(inputString, "ERROR");
        }

    }

//////////////////////////////////////////////////////////////////////////

    switch(state){
        case INIT:
            if(strncmp(inputString,"WIFI GOT IP",11) == 0){
                state = RECONNECT;
//                memset(inputString, 0, strlen(inputString));
                inputString[0] = '\0';
            }
            else if(strncmp(inputString,"WIFI CONNECTED",14) == 0){
                inputString[0] = '\0';
            }
            break;
        case RX_DATA:
            if(overflow == TRUE){
                strcpy(txData,"Overflow");
                state = TX_DATA;
                txState = TX_SETUP;
                overflow = FALSE;
            }
            else{
                if(strncmp(inputString,"+IPD,",5) == 0){
                    char rxStr[5] = "";
                    int rxAmmount = 0;

                    while(rxAmmount == 0){
                       if(inBuffer.head != inBuffer.tail){
                           popFromBuffer(&inBuffer, &outChar);
                           int rxStrLen = strlen(rxStr);
                           if(outChar == ':'){
                               rxAmmount = atoi(rxStr);
                               if(rxAmmount == 1){
                                   strcpy(txData, "No Data");
                                   state = TX_DATA;
                                   txState = TX_SETUP;
                                   return;
                               }
                           }
                           else if(rxStrLen < 3){
                               rxStr[rxStrLen] = outChar;
                               rxStr[rxStrLen+1] = '\0';
                           }
                           else{
                               overflow = TRUE;
                               return;
                           }
                        }
                    }

                    if(rxAmmount <= RX_STRING_LENGTH){
                        int count = 0;
                        txData[0] = '\0';
                        //TODO: Could get stuck here if transmission gets garbled
                        while(count < rxAmmount){
                            if(inBuffer.head != inBuffer.tail){
                                popFromBuffer(&inBuffer, &outChar);
                                count++;
                                int tmpLen = strlen(txData);
                                txData[tmpLen] = outChar;
                                txData[tmpLen + 1] = '\0';
                            }
                        }
                        state = TX_DATA;
                        txState = TX_SETUP;
                    }
                    else{
                        overflow = TRUE;
                    }
                }
            }
            break;
        case TX_DATA:
            switch(txState){
                case TX_SETUP:
                    if(strlen(txData) > 0){
                        static char tmpNumber[10];
                        strcpy(txTmpString, "AT+CIPSEND=");
                        itoa(strlen(txData),tmpNumber,10);
                        strcat(txTmpString, tmpNumber);
                        strcat(txTmpString, "\r\n");
                        SerialWrite(txTmpString);
                        txState = TX_VALIDATE;
                        sendComplete = FALSE;
                    }
                    break;
                case TX_VALIDATE:
                    if(strncmp(inputString,"OK",2) == 0){
                        txState = TX_SEND;
//                        memset(inputString, 0, strlen(inputString));
                        inputString[0] = '\0';
                    }
                    else if(strncmp(inputString,"ERROR",5) == 0){
                        txState = TX_SETUP;
//                        memset(inputString, 0, strlen(inputString));
                        inputString[0] = '\0';

                    }
//                    else if(strncmp(inputString,"busy s...",9) == 0){
//                        txState = TX_SETUP;
////                        memset(inputString, 0, strlen(inputString));
//                        inputString[0] = '\0';
//                    }

                    break;
                case TX_SEND:
                    if(inChar == '>'){
                        SerialWrite(txData);
                        txState = TX_VALIDATE_SEND;
                        inputString[0] = '\0';
                    }
                    break;
                case TX_VALIDATE_SEND:
                        if(strncmp(inputString,"SEND OK",7) == 0){
                            txState = TX_SETUP;
                            state = RX_DATA;
                            sendComplete = TRUE;
//                            memset(inputString, 0, strlen(inputString));
                            inputString[0] = '\0';

                            if(overflow == TRUE){

                                overflow = FALSE;

                                if(strncmp(txData,"Overflow",8) == 0){
                                    state = RX_DATA;
                                    txState = TX_SETUP;
                                }
                                else{
                                    strcpy(txData,"Overflow");
                                    state = TX_DATA;
                                    txState = TX_SETUP;
                                }
                            }
                        }
                        else if(strncmp(inputString,"SEND FAIL",9) == 0){
                            //TODO: Do something here
                        }
                    break;
                default:
                    break;
            }
            break;
        case RECONNECT:
            switch(reconnectState){
                case RC_CONNECT:
                    strcpy(txTmpString, "AT+CIPSTART=\"TCP\",\"");
                    strcat(txTmpString, SERVER_IP);
                    strcat(txTmpString, "\",5000\r\n");
                    reconnectState = RC_VERIFY;
                    SerialWrite(txTmpString);
                    break;
                case RC_VERIFY:
                    if(strncmp(inputString,"OK",2) == 0){
                        state = RX_DATA;
                        connected = TRUE;
                        reconnectState = RC_CONNECT;
//                        memset(inputString, 0, strlen(inputString));
                        inputString[0] = '\0';
                    }
                    else if(strcmp(inputString,"CLOSED") == 0){
                        if(inputString[1] == 'I'){
                            state = 0;
                        }
                        reconnectState = RC_CONNECT;
//                        memset(inputString, 0, strlen(inputString));
                        inputString[0] = '\0';
                    }
                    //TODO: Should also be posible to get an "ERROR" message here if transmission gets garbled
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

long TimeSinceBoot(){
    return msSinceBoot;
}

int SendData(char *Data){
    if(state == RX_DATA){
        if(strlen(Data) < MAX_TX_DATA){
            strcpy(txData, Data);
            state = TX_DATA;
            txState = TX_SETUP;
            return 0;
        }
        else{
            return -1;
        }
    }
    else{
        return -1;
    }
}

static void SerialWrite(char *TxArray){
    unsigned char ArrayLength = strlen(TxArray);
    while(ArrayLength--){
        while(!(IFG2 & UCA0TXIFG));
        UCA0TXBUF = *TxArray;
        TxArray++;
    }
}

static int pushToBuffer(struct ringBuffer *b, const char inChar){
    if(b->head == RX_STRING_LENGTH-1){
        b->head = 0;
    }
    else{
        b->head++;
    }

    if(b->head != b->tail){
        b->buffer[b->head] = inChar;
        return 0;
    }
    else{
        b->head--;
        return -1;
    }
}

static int popFromBuffer(struct ringBuffer *b, char *outChar){
    if(b->tail != b->head){
        if(b->tail == RX_STRING_LENGTH-1){
            b->tail = 0;
        }
        else{
            b->tail++;
        }

        *outChar = b->buffer[b->tail];
        return 0;
    }
    else{
        //Head equals tail, therefore nothing on the buffer
        return -1;
    }
}

void __attribute__((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR(void)
{
    inChar = UCA0RXBUF;

    //Check to make sure inputString is an ASCII char and not null
    if(((inChar & ~0x7F) == 0) && (inChar != '\0')){
        pushToBuffer(&inBuffer, inChar);
    }
}

void __attribute__((interrupt(TIMER0_A0_VECTOR))) TIMER_A_ISR(void)
{
    msSinceBoot++;
}
