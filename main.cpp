/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"


// Blinking rate in milliseconds
#define BLINKING_RATE     500ms

// Initialise the digital pin LED1 as an output
#ifdef LED1
    DigitalOut led(LED1);
#else
    bool led;
#endif


// Initialise the digital pin BUTTON1 as an input
#ifdef BUTTON1
    //DigitalIn sw(BUTTON1);
    InterruptIn sw(BUTTON1);
#else
    bool sw;
#endif


volatile bool button_clicked = false;  // Flag pour indiquer que l'interruption s'est produite


void flip()
{
    led = !led;
    button_clicked = true;
}

int main()
{
    sw.mode(PullDown);
    sw.rise(&flip);

    while (true)
    {

        if (button_clicked) {
            printf("\nBouton pressé, LED inversée\n\n");
            button_clicked = false; 
        }

        // Si utilisation de sw comme un DigitalIn
        /*if (sw)
            led = 1;
        else
            led = 0; */
        

        printf("hello world\n");
        ThisThread::sleep_for(BLINKING_RATE);
    }
}
