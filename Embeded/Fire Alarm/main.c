#include <msp430.h> 
#include<wifi.h>
/*
 * main.c
 */
int main(void) {
    WifiSetup();
    while(1)
    {
        WifiLoop();
    }
	
	return 0;
}
