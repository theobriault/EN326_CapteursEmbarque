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




int getDp()
{
    char cmd[3] = {};
    cmd[0] = 0xFC;
    cmd[1] = 0xFB;
    cmd[2] = 0xFA;

    char data[3] = {};
    i2c.write(addr8bit_pressure, cmd, 3);
    i2c.read(addr8bit_pressure, data, 3);
    
    if (data[2] >> 7 == 1)
        printf("laaaaa\n");
    
    if (data[0] >> 7 == 1)
        printf("looooo\n");

    if (data[1] >> 7 == 1)
        printf("liiiiii\n");
    
    
    //~data[2] + 1;


    //printf("1:%d\n", val_data);
    //printf("2:%d\n", data[1]);
    //printf("3:%d\n\n", data[2]);

    //int Dp = float((data[2] << 16) | (data[1] << 8) | (data[0])) - 8388607;
    //printf("Dp : %d \n\n", Dp);
    return 0;
    
}


int getDt()
{
    char cmd[3] = {};
    cmd[0] = 0xF9;
    cmd[1] = 0xF8;
    cmd[2] = 0xF7;

    char data[3] = {};
    i2c.write(addr8bit_pressure, cmd, 1);
    i2c.read(addr8bit_pressure, data, 3);
    int Dt = float((data[2] << 16) | (data[1] << 8) | (data[0])) - 8388607; 
    printf("Dt : %d \n\n", Dt);
    return Dt;
    //printf("pressure : %d Pa\n\n", Dp);
}


int getTr(int Dt)
{
    // get coeff a
    char cmd[6] = {};
    cmd[0] = 0xB2;
    cmd[1] = 0xB3;
    cmd[2] = 0xB4;
    cmd[3] = 0xB5;    
    cmd[4] = 0xB6;
    cmd[5] = 0xB7;
    //cmd[2] = 0xF7;

    char data[6] = {};
    i2c.write(addr8bit_pressure, cmd, 6);
    i2c.read(addr8bit_pressure, data, 6);
    double a0 =  data[0] << 8 | data[1]; //coefficient à ajouter
    double a1 =  data[2] << 8 | data[3];
    double a2 =  data[4] << 8 | data[5];
    int Tr = a0 + a1*Dt + a2*pow(Dt, 2);
    return 0;
    //printf("pressure : %d Pa\n\n", Dp);
}

int getCoeffs(int Dp, int Dt, int Tr)
{
    char cmd[18] = {};
    cmd[0] = 0xA0;
    cmd[1] = 0xA1;
    cmd[2] = 0xA2;
    cmd[3] = 0xa3;    
    cmd[4] = 0xA4;
    cmd[5] = 0xA5;
    cmd[6] = 0xA6;
    cmd[7] = 0xA7;
    cmd[8] = 0xA8;
    cmd[9] = 0xA9;    
    cmd[10] = 0xAA;
    cmd[11] = 0xAB;
    cmd[12] = 0xAC;
    cmd[13] = 0xAD;
    cmd[14] = 0xAE;
    cmd[15] = 0xAF;    
    cmd[16] = 0xB0;
    cmd[17] = 0xB1;

    char data[18] = {};
    i2c.write(addr8bit_pressure, cmd, 18);
    i2c.read(addr8bit_pressure, data, 18);

    double b00 = data[0] << 8 | data[1];
    double bt1 = data[2] << 8 | data[3];
    double bt2 = data[4] << 8 | data[5];
    double bp1 = data[6] << 8 | data[7];
    double b11 = data[8] << 8 | data[9];
    double bp2 = data[10] << 8 | data[11];
    double b12 = data[12] << 8 | data[13];
    double b21 = data[14] << 8 | data[15];
    double bp3 = data[16] << 8 | data[17];

    int Pr = b00 + Tr*bt1 + bp1*Dp + b11 * Dp * Tr + bt2 * pow(Tr, 2) + bp2 * pow(Dp, 2) + b12 * Dp * pow(Tr, 2) + b21 * pow(Dp, 2) * Tr +bp3 * pow(Dp, 3);
    return Pr;
}


int main()
{
    while (1) {
        //getTemperature();
        // getHumidity();



        int Dp = getDp();
        //int Dt = getDt();
        //int Tr = getTr(Dt);
        // int Pr = getCoeffs(Dp, Dt, Tr);
        // printf("Pressure : %d\n", Pr);
        ThisThread::sleep_for(BLINKING_RATE);
    }
}


