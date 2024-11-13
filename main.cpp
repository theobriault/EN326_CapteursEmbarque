/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"


// Blinking rate in milliseconds
#define BLINKING_RATE     500ms


I2C i2c(P1_I2C_SDA, P1_I2C_SCL);


const int addr7bit_temperature = 0x48;      // 7 bit I2C address
const int addr8bit_temperature = addr7bit_temperature << 1; // 8bit I2C address, 0x90

const int addr7bit_humidity = 0x40;      // 7 bit I2C address
const int addr8bit_humidity = addr7bit_humidity << 1; 

const int addr7bit_pressure = 0x70;      // 7 bit I2C address
const int addr8bit_pressure = addr7bit_pressure << 1; 


void getTemperature()
{
    char cmd[2];

    cmd[0] = 0x00;
    i2c.write(addr8bit_temperature, cmd, 1);
    i2c.read(addr8bit_temperature, cmd, 2);
    int temperature_degC = (float((cmd[0] << 8) | cmd[1]) / 128.0);
    printf("temperature : %d degC\n", temperature_degC);
}



void getHumidity()
{
    char cmd[2];

    cmd[0] = 0xE5;
    i2c.write(addr8bit_humidity, cmd, 1);
    i2c.read(addr8bit_humidity, cmd, 2);
    int humidity = -6 + 125 * (float((cmd[0] << 8) | cmd[1]) / 65535);
    printf("humidity : %d%%\n", humidity);
}




void getPressure()
{
    char cmd[3] = {};
    cmd[0] = 0xF9;
    // cmd[1] = 0xF8;
    // cmd[2] = 0xF7;

    char data[3] = {};
    i2c.write(addr8bit_pressure, cmd, 1);
    i2c.read(addr8bit_pressure, data, 3);
    int pressure = float((data[2] << 16) | (data[1] << 8) | (data[0])) - 8388607;
    printf("pressure : %d Pa\n\n", pressure);
}





int main()
{
    while (1) {
        getTemperature();
        getHumidity();
        getPressure();
        ThisThread::sleep_for(BLINKING_RATE);
    }
}