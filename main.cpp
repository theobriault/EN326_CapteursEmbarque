/*
 * Copyright (c) 2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"


DigitalOut led1(LED1);
DigitalOut led2(LED2);
Thread thread1(osPriorityNormal, 1024, nullptr, nullptr);
Thread thread2(osPriorityNormal, 1024, nullptr, nullptr);

/*void led2_thread()
{
    while (true) {
        led2 = !led2;
        ThisThread::sleep_for(1s);
    }
}
*/

void ping(void)
{
    for(int i = 0; i<100; i++)
        printf("Ping\n");
}

void pong(void)
{
    for(int i = 0; i<100; i++)
        printf("Pong\n");
}

int main()
{
//    thread.start(led2_thread);
    thread1.start(ping);
    thread2.start(pong);



    while (true) {
        led1 = !led1;
        ThisThread::sleep_for(500);
    }

}


