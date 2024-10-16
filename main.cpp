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

volatile bool count_finished = false;
volatile bool button_clicked = false;  // Flag pour indiquer que l'interruption s'est produite
using namespace std::chrono;    
Timer t;

void UpRoutine()
{
    led = !led;
    button_clicked = true;
    t.reset();
    t.start();

}

void DownRoutine()
{
    t.stop();
    count_finished = true;
}

int main()
{
    //sw.mode(PullDown);
    sw.rise(&UpRoutine);
    sw.fall(&DownRoutine);

    while (true)
    {
        //Attente active (polling)
        // Si utilisation de sw comme un DigitalIn
        /*if (sw)
            led = 1;
        else
            led = 0; */



        if (button_clicked) 
        {
            printf("\nBouton pressé, LED inversée\n");
            button_clicked = false; 
        }
                
        if(count_finished)
        {
            printf("Duree appui : %llums\n", duration_cast<milliseconds>(t.elapsed_time()).count());
            count_finished = false;
        }
  
        //printf("hello world\n");
        ThisThread::sleep_for(BLINKING_RATE);
    }
}
