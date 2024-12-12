/*
 * Copyright (c) 2021, Koncepto.io
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef CATIE_SIXTRON_HTU21D_H_
#define CATIE_SIXTRON_HTU21D_H_

#include "mbed.h"

namespace sixtron {

/*!
 * \brief Measurement resolution
 */
enum class MeasurementResolution : char {
    /** RH 12 bits, Temperature 14 bits */
    RH_12_Temp_14 = 0,
    /** RH 8 bits, Temperature 12 bits */
    RH_8_Temp_12 = 1,
    /** RH 10 bits, Temperature 13 bits */
    RH_10_Temp_13 = 2,
    /** RH 11 bits, Temperature 11 bits */
    RH_11_Temp_11 = 3,
};

/*!
 * \brief User register data
 */
typedef struct {
    /** Measurement resolution */
    MeasurementResolution resolution;
    /** Low voltage status */
    bool end_of_battery;
    /** Heater enable */
    bool enable_heater;
    /** OTP reload disable */
    bool disable_otp_reload;
} htud21d_user_reg_t;

/*!
 *  \class HTU21D
 *  HTU21D(F) humidity sensor driver
 */
class HTU21D {
public:
    /*!
     * \brief Commands error codes
     */
    enum class ErrorType : uint8_t {
        /** Command successful */
        Ok = 0,
        /** Error returned by I2C communication */
        I2cError = 1,
        /** Diagnostic state (open circuit) reported by sensor */
        DiagStateOpenCircuit = 2,
        /** Diagnostic state (short circuit) reported by sensor */
        DiagStateShortCircuit = 3,
        /** Checksum error in received frame */
        ChecksumError = 4,
    };

    /*! Constructor
     *
     * \param i2c_sda I2C SDA pin
     * \param i2c_scl I2C SCL pin
     */
    HTU21D(PinName i2c_sda, PinName i2c_scl);

    /*!
     * \brief Read temperature in Celsius degrees
     *
     * \param result pointer to location to store result
     *
     * \return Ok on success, the reason of failure otherwise
     */
    ErrorType read_temperature(double *result);

    /*!
     * \brief Read relative humidity in percent
     *
     * \param result pointer to location to store result
     *
     * \return Ok on success, the reason of failure otherwise
     */
    ErrorType read_humidity(double *result);

    /*!
     * \brief Perform a soft reset
     *
     * \return Ok on success, the reason of failure otherwise
     */
    ErrorType soft_reset();

    /*!
     * \brief Read user register
     *
     * \param reg Pointer to location to store read data
     *
     * \return Ok on success, the reason of failure otherwise
     */
    ErrorType read_user_register(htud21d_user_reg_t *reg);

    /*!
     * \brief Write user register
     *
     * \param reg Pointer to data to write
     *
     * \return Ok on success, the reason of failure otherwise
     */
    ErrorType write_user_register(htud21d_user_reg_t *reg);

private:
    I2C _bus;

    /*!
     * \brief Sensor commands
     */
    enum class CmdList : char {
        /** Trigger Temperature Measurement with master clock hold */
        TrigTemperatureMeasHold = 0xE3,
        /** Trigger Humidity Measurement with master clock hold */
        TrigHumidityMeasHold = 0xE5,
        /** Trigger Temperature Measurement without master clock hold */
        TrigTemperatureMeasNoHold = 0xF3,
        /** Trigger Humidity Measurement without master clock hold */
        TrigHumidityMeasNoHold = 0xF5,
        /** Write user register */
        WriteUserReg = 0xE6,
        /** Read user register */
        ReadUserReg = 0xE7,
        /** Perform a soft reset */
        SoftReset = 0xFE,
    };

    /*!
     * \brief Read the raw value of user register
     *
     * \param reg Pointer to location to store the register raw value
     * \param repeated Repeated start, true - do not send stop at end
     *         default value is false.
     *
     * \return Ok on success, the reason of failure otherwise
     */
    ErrorType read_user_register_raw(char *reg, bool repeated = false);

    /*!
     * \brief Write raw value of user register
     *
     * \param reg Register raw value
     *
     * \return Ok on success, the reason of failure otherwise
     */
    ErrorType write_user_register_raw(char reg);

    /*!
     * \brief Read a raw value from the sensor.
     *        Works with humidity or temperature measurement
     *        with master clock hold.
     *
     * \param cmd Command to send
     * \param result Location to store read raw value
     *
     * \return Ok on success, the reason of failure otherwise
     */
    ErrorType read_value_raw(CmdList cmd, uint16_t *result);

    /*!
     * \brief Compute CRC of a given message
     *
     * \param data data buffer to compute the CRC on
     * \param len length of the data buffer
     *
     * \return computed value of CRC
     */
    char compute_crc(char *data, uint8_t len);
};

} // namespace sixtron

#endif // CATIE_SIXTRON_HTU21D_H_
