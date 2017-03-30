/*
 * main.c
 */
#include <msp430.h>
#include <wifi.h>

int main(void) {
	WifiSetup();
	
	while(1){
	    WifiLoop();
	}
	return 0;
}
