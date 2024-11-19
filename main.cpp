/*
 * Copyright (c) 2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"

#define BLINKING_RATE     500ms


// Initialisation des LEDs
DigitalOut led1(LED1);
DigitalOut led2(LED2);

// Mutex pour protéger l'accès à stdout
Mutex stdio_mutex;

// Threads pour exécuter des tâches en parallèle
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
    for(int i = 0; i<100; i++){
        stdio_mutex.lock();
        printf("Ping\n");
        stdio_mutex.unlock();
    }
}

void pong(void)
{
    for(int i = 0; i<100; i++){
        stdio_mutex.lock();
        printf("Pong\n");
        stdio_mutex.unlock();
    }
}

int main()
{
    // Lancement des threads
    // thread.start(led2_thread);
    thread1.start(ping);
    thread2.start(pong);



    while (true) {
        led1 = !led1;
        ThisThread::sleep_for(BLINKING_RATE);
    }

}


