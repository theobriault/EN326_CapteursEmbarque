/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
using namespace std::chrono;


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
    InterruptIn sw(BUTTON1);
#else
    bool sw;
#endif

volatile bool count_finished = false;
volatile bool button_clicked = false;  // Flag pour indiquer que l'interruption s'est produite
volatile int idx = 0; // index pour le tableau des différentes fréquence
Timer t;
Ticker flipper;

void ledToggle()
{
    led = !led;
}

void changeFrequency()
{
    double frequencies[5] = {4.0, 3.0, 2.0, 1.0, 0.5};
    idx = (idx + 1) % 5;  
    flipper.attach(&ledToggle, frequencies[idx]);  
    button_clicked = true;
}

int main()
{
    sw.rise(&changeFrequency);
    flipper.attach(&ledToggle, 5.0); // the address of the function to be attached (flip) and the interval (2 seconds)

    while (true)
    {

        if (button_clicked) 
        {
            printf("\nBouton pressé, Changement de fréquence %d/5\n", idx+1);
            button_clicked = false; 
        }
                  
        ThisThread::sleep_for(BLINKING_RATE);
    }
}
