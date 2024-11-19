#include "I2CDevice.hpp"


I2CDevice::I2CDevice(PinName sda, PinName scl)
    : i2c(sda, scl)
{}



bool I2CDevice::write(uint8_t reg, const uint8_t* data, size_t length)
{
    return 0;
}



bool I2CDevice::read(uint8_t reg, uint8_t* buffer, size_t length)
{
    return 0;
}