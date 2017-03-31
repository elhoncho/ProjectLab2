/*******************************************************************/
/* Things to do                                                    */
/*     Will get a double overflow message on large overflows       */
/*     Debug that weird case marked below                          */
/*     Need to have a fail safe to break from the while loops      */
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

#define RX_STRING_LENGTH 50


static const char SERVER_IP[] = "10.0.0.6";

static int returnState = 0;

static volatile char inChar;

struct ringBuffer{
    volatile int head;
    int tail;
    volatile char buffer[RX_STRING_LENGTH];
};

static struct ringBuffer inBuffer;

static void SerialWrite(char *TxArray);
static int pushToBuffer(struct ringBuffer *b, const char inChar);
static int popFromBuffer(struct ringBuffer *b, char *outChar);

void WifiSetup(){
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    //Setup the clock
    DCOCTL = 0; // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ; // Set DCO to 16MHz
    DCOCTL = CALDCO_16MHZ; // Set DCO to 16MHz

    P2DIR |= 0XFF;
    P2OUT &= 0X00;

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

    static int state = 0;
    static int txState = 0;
    static int reconnectState = 0;

    static char inputString[RX_STRING_LENGTH] = "";
    static char parseBuffer[RX_STRING_LENGTH] = "";
    static char txData[RX_STRING_LENGTH] = "";
    static char tmpString[RX_STRING_LENGTH] = "";


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
            returnState = 0;
            connected = FALSE;
            reconnectState = 0;
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
//                parseBuffer[RX_STRING_LENGTH-2] = '\0';
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

                    char *strPtr = strchr(inputString, ':');

                    if(strPtr != NULL){
                        inputString[0] = '\0';
                        strncpy(rxStr, &inputString[5], strPtr-&inputString[5]);
                        itoa(rxAmmount, rxStr,10);

                        if(rxAmmount >= 1000){
                            overflow = TRUE;
                           return;
                        }
                    }
                    else if(inputString[5] != '\0' && strPtr == NULL){
                        inputString[0] = '\0';
                        //TODO: Don't know if this works, weird case where it has received only part of the string to send but not the : yet
                        strcpy(rxStr, &inputString[5]);
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
                    }
                    else{
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
                        strcpy(tmpString, "AT+CIPSEND=");
                        itoa(strlen(txData),tmpNumber,10);
                        strcat(tmpString, tmpNumber);
                        strcat(tmpString, "\r\n");
                        SerialWrite(tmpString);
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
                                strcpy(txData,"Overflow");
                                state = TX_DATA;
                                txState = TX_SETUP;
                                overflow = FALSE;
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
                    strcpy(tmpString, "AT+CIPSTART=\"TCP\",\"");
                    strcat(tmpString, SERVER_IP);
                    strcat(tmpString, "\",80\r\n");
                    reconnectState = RC_VERIFY;
                    SerialWrite(tmpString);
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
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
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
