/*
 * Copyright (c) 2021, Koncepto.io
 * SPDX-License-Identifier: Apache-2.0
 */
#include "HTU21D.h"

/* Device address on 8 bits */
#define HTU21D_ADDR (0x40 << 1)

/* CRC parameters for use with MbedCRC */
#define HTU21D_POLY (0x31)
#define HTU21D_WIDTH_POLY (8)
#define HTU21D_INIT_XOR (0x00)
#define HTU21D_FINAL_XOR (0)
#define HTU21D_REFLECT_IN (false)
#define HTU21D_REFLECT_OUT (false)

/* User register bits */
#define HTU21D_RESOLUTION_UPPER_BIT (7)
#define HTU21D_END_OF_BATT_BIT (6)
#define HTU21D_HEATER_BIT (2)
#define HTU21D_OTP_RELOAD_BIT (1)
#define HTU21D_RESOLUTION_LOWER_BIT (0)

/* Mask to keep read-only values and reserved bits */
#define HTU21D_USER_REG_KEEP_MASK (0x78)

namespace sixtron {

HTU21D::HTU21D(PinName i2c_sda, PinName i2c_scl): _bus(i2c_sda, i2c_scl)
{
}

HTU21D::ErrorType HTU21D::soft_reset()
{
    ErrorType err = ErrorType::Ok;
    const char buf = static_cast<char>(CmdList::SoftReset);

    this->_bus.lock();
    if (this->_bus.write(HTU21D_ADDR, &buf, 1)) {
        err = ErrorType::I2cError;
    }
    this->_bus.unlock();

    return err;
}

HTU21D::ErrorType HTU21D::read_temperature(double *result)
{
    ErrorType err;
    uint16_t raw_value;

    this->_bus.lock();
    err = this->read_value_raw(CmdList::TrigTemperatureMeasHold, &raw_value);
    this->_bus.unlock();

    if (err == ErrorType::Ok) {
        *result = -46.85 + 175.72 * (double)raw_value / (1 << 16);
    }

    return err;
}

HTU21D::ErrorType HTU21D::read_user_register(htud21d_user_reg_t *reg)
{
    ErrorType err;
    char tmp;

    this->_bus.lock();
    err = this->read_user_register_raw(&tmp);
    this->_bus.unlock();

    if (err == ErrorType::Ok) {
        reg->disable_otp_reload = (tmp >> HTU21D_OTP_RELOAD_BIT) & 0x1;
        reg->enable_heater = (tmp >> HTU21D_HEATER_BIT) & 0x1;
        reg->end_of_battery = (tmp >> HTU21D_END_OF_BATT_BIT) & 0x1;
        reg->resolution = static_cast<MeasurementResolution>(
                (((tmp >> HTU21D_RESOLUTION_UPPER_BIT) & 0x1) << 1)
                | ((tmp >> HTU21D_RESOLUTION_LOWER_BIT) & 0x1));
    }

    return err;
}

HTU21D::ErrorType HTU21D::write_user_register(htud21d_user_reg_t *reg)
{
    ErrorType err;
    char tmp;

    this->_bus.lock();
    err = this->read_user_register_raw(&tmp);

    if (err != ErrorType::Ok) {
        goto write_user_register_end;
    }

    tmp &= HTU21D_USER_REG_KEEP_MASK;

    tmp |= reg->disable_otp_reload ? (1 << HTU21D_OTP_RELOAD_BIT) : 0;
    tmp |= reg->enable_heater ? (1 << HTU21D_HEATER_BIT) : 0;

    tmp |= ((static_cast<char>(reg->resolution) >> 1) & 0x1) << HTU21D_RESOLUTION_UPPER_BIT;
    tmp |= (static_cast<char>(reg->resolution) & 0x1) << HTU21D_RESOLUTION_LOWER_BIT;

    err = this->write_user_register_raw(tmp);

write_user_register_end:
    this->_bus.unlock();
    return err;
}

HTU21D::ErrorType HTU21D::read_humidity(double *result)
{
    ErrorType err;
    uint16_t raw_value;

    this->_bus.lock();
    err = this->read_value_raw(CmdList::TrigHumidityMeasHold, &raw_value);
    this->_bus.unlock();

    if (err == ErrorType::Ok) {
        *result = -6 + 125 * (double)raw_value / (1 << 16);
    }

    return err;
}

HTU21D::ErrorType HTU21D::read_user_register_raw(char *reg, bool repeated)
{
    ErrorType err = ErrorType::Ok;
    char tmp = static_cast<char>(CmdList::ReadUserReg);

    if (this->_bus.write(HTU21D_ADDR, &tmp, 1, true)) {
        err = ErrorType::I2cError;
        this->_bus.stop();
        goto read_user_register_raw_end;
    }

    if (this->_bus.read(HTU21D_ADDR, reg, 1, repeated)) {
        if (repeated) {
            this->_bus.stop();
        }
        err = ErrorType::I2cError;
    }

read_user_register_raw_end:
    return err;
}

HTU21D::ErrorType HTU21D::write_user_register_raw(char reg)
{
    ErrorType err = ErrorType::Ok;
    char buf[2] = { static_cast<char>(CmdList::WriteUserReg), reg };

    if (this->_bus.write(HTU21D_ADDR, buf, 2)) {
        err = ErrorType::I2cError;
        this->_bus.stop();
    }

    return err;
}

HTU21D::ErrorType HTU21D::read_value_raw(CmdList cmd, uint16_t *result)
{
    ErrorType err = ErrorType::Ok;
    char buf[3];

    assert((cmd == CmdList::TrigTemperatureMeasHold) || (cmd == CmdList::TrigHumidityMeasHold));

    buf[0] = static_cast<char>(cmd);
    if (this->_bus.write(HTU21D_ADDR, buf, 1, true)) {
        err = ErrorType::I2cError;
        this->_bus.stop();
        goto read_raw_value_end;
    }

    if (this->_bus.read(HTU21D_ADDR, buf, 3)) {
        err = ErrorType::I2cError;
        goto read_raw_value_end;
    }

    if (0 != this->compute_crc(buf, 3)) {
        err = ErrorType::ChecksumError;
        goto read_raw_value_end;
    }

    if ((buf[0] == 0x00) && (buf[1] == 0x00)) {
        err = ErrorType::DiagStateOpenCircuit;
        goto read_raw_value_end;
    }

    if ((buf[0] == 0xFF) && (buf[1] == 0xFF)) {
        err = ErrorType::DiagStateShortCircuit;
        goto read_raw_value_end;
    }

    /* Merge data and remove status bits */
    *result = (((uint16_t)buf[0] << 8) | (buf[1] & 0xFC));

read_raw_value_end:
    return err;
}

char HTU21D::compute_crc(char *data, uint8_t len)
{
    uint32_t crc;
    MbedCRC<HTU21D_POLY, HTU21D_WIDTH_POLY> ct(
            HTU21D_INIT_XOR, HTU21D_FINAL_XOR, HTU21D_REFLECT_IN, HTU21D_REFLECT_OUT);

    ct.compute(data, len, &crc);
    return (crc & 0xFF);
}

} // namespace sixtron
