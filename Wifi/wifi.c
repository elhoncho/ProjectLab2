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

static const char SERVER_IP[] = "10.0.0.7";

static int connected = FALSE;

static int state = 0;
static int txState = 0;
static int returnState = 0;
static int reconnectState = 0;

static char inputString[RX_STRING_LENGTH] = "";
static char txData[RX_STRING_LENGTH] = "";
static char tmpString[RX_STRING_LENGTH] = "";
static char tmpNumber[10] = "";
static char tmpInputString[RX_STRING_LENGTH] = "";

static volatile char inChar;
static void SerialWrite(char *TxArray);

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
}

void WifiLoop(){

    ////////////////////////////////////////////////////////////
    //                  Parse the input string                //
    ////////////////////////////////////////////////////////////
    if(endsWith(tmpInputString, "\n")){
        //Remove the \r\n from the command
        strcpy(inputString, tmpInputString);
        trimString(inputString);
        //memset(tmpInputString, 0, strlen(tmpInputString));
        tmpInputString[0] = '\0';

        if(strncmp(inputString,"ready",5) == 0){
            state = 0;
            txState = 0;
            returnState = 0;
            connected = FALSE;
            reconnectState = 0;
    //        memset(inputString, 0, strlen(inputString));
            inputString[0] = '\0';
        }
        else if(strncmp(inputString,"CLOSED",6) == 0 && connected){
            state = RECONNECT;
            connected = FALSE;
            reconnectState = 0;
    //        memset(inputString, 0, strlen(inputString));
            inputString[0] = '\0';
        }
        else if(strncmp(tmpInputString,"+IPD",4)== 0){
            trimString(tmpInputString);
            strcpy(inputString, tmpInputString);
            tmpInputString[0] = '\0';
        }
        else if(strncmp(inputString,"busy s...",9) == 0){
            //Retransmit
    //        memset(inputString, 0, strlen(inputString));
            inputString[0] = '\0';
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
        break;
        case RX_DATA:
            if(strncmp(inputString,"+IPD",4) == 0){
                //-3 for /r /n and /0
                strncpy(txData, strchr(inputString, ':')+1,RX_STRING_LENGTH-3);
                state = TX_DATA;
//                memset(inputString, 0, strlen(inputString));
                inputString[0] = '\0';
            }
        break;
        case TX_DATA:
            switch(txState){
                case TX_SETUP:
                    if(strlen(txData) > 0){
                        strcpy(tmpString, "AT+CIPSEND=");
                        itoa(strlen(txData),tmpNumber,10);
                        strcat(tmpString, tmpNumber);
                        strcat(tmpString, "\r\n");
                        SerialWrite(tmpString);
                        txState = TX_VALIDATE;
                    }
                    else{
                        strcpy(txData,"ERROR: No data");
                        txState = TX_SETUP;
                    }
                    break;
                case TX_VALIDATE:
                    if(strncmp(inputString,"OK",2) == 0){
                        txState = TX_SEND;
//                        memset(inputString, 0, strlen(inputString));
                        inputString[0] = '\0';
                    }
                    else if(strncmp(inputString,"busy s...",9) == 0){
                        txState = TX_SETUP;
//                        memset(inputString, 0, strlen(inputString));
                        inputString[0] = '\0';
                    }
                    else if(strncmp(inputString,"ERROR",5) == 0){
                        txState = TX_SETUP;
//                        memset(inputString, 0, strlen(inputString));
                        inputString[0] = '\0';
                    }
                    break;
                case TX_SEND:
                    if(inChar == '>'){
                        SerialWrite(txData);
                        txState = TX_VALIDATE_SEND;
//                        memset(txData, 0, strlen(txData));
                        inputString[0] = '\0';
                    }
                    break;
                case TX_VALIDATE_SEND:
                        if(strncmp(inputString,"SEND OK",7) == 0){
                            txState = TX_SETUP;
                            state = RX_DATA;
//                            memset(inputString, 0, strlen(inputString));
                            inputString[0] = '\0';
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
                    //Must be the last thing called, it will jump out and modify the state
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
                    //TODO: WIFI GOT IP some how passes the strcmp with CLOSED
                    else if(strcmp(inputString,"CLOSED") == 0){
                        int i = strcmp(inputString, "CLOSED");
                        char tmpChar = inputString[i+1];
                        if(tmpChar == 'j')
                            reconnectState = 1;
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
    while(ArrayLength--){ // Loop until StringLength == 0 and post decrement
        while(!(IFG2 & UCA0TXIFG)); // Wait for TX buffer to be ready for new data
        UCA0TXBUF = *TxArray; //Write the character at the location specified py the pointer
        TxArray++; //Increment the TxString pointer to point to the next character
    }
}


void __attribute__((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR(void)
{
    inChar = UCA0RXBUF;

    int len = strlen(tmpInputString);

    //Check to make sure inputString isnt overflown and that the char is an ASCII char
    if((len < RX_STRING_LENGTH-1) && ((inChar & ~0x7F) == 0) && (inChar != '\0')){
        tmpInputString[len] = inChar;
        tmpInputString[len+1] = '\0';
    }

    if(len == RX_STRING_LENGTH-1){
        if(strncmp(tmpInputString,"+IPD",4)== 0){
            strcpy(txData, "Overflow");
            state = TX_DATA;
        }
        //memset(tmpInputString, 0, len);
        tmpInputString[0] = '\0';
    }
}
