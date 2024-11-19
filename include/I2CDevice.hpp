#ifndef I2CDEVICE_HPP
#define I2CDEVICE_HPP

#include "mbed.h"


class I2CDevice {
    public:
        I2CDevice(PinName sda, PinName scl);

        /*
         *
         */
        bool write(uint8_t reg, const uint8_t* data, size_t length);


        /*
         *
         */
        bool read(uint8_t reg, uint8_t* buffer, size_t length);
    
    private:
        I2C i2c;




};




#endif // I2CDEVICE_HPP