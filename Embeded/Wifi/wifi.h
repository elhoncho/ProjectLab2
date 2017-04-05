/*
 * wifi.h
 *
 *  Created on: Mar 29, 2017
 *      Author: Denton
 */

//

#ifndef WIFI_H_
#define WIFI_H_

    void WifiSetup();
    void WifiLoop();
    long TimeSinceBoot();
    int SendData(char *Data);
    volatile char rxData[];
    int NewData();

#endif /* WIFI_H_ */
