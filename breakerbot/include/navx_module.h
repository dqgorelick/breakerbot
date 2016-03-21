#include "mraa.h"

#ifndef NAVX_MODULE_H
#define NAVX_MODULE_H

class NavX_Module {
public:
    NavX_Module() {};
    float get_yaw() {
        mraa_i2c_context i2c = mraa_i2c_init(6);
        mraa_result_t result = MRAA_SUCCESS;
        result = mraa_i2c_address(i2c, 0x32);
        printf((result != MRAA_SUCCESS) ? "was not able to connect to address\n", "Was able to write to the device\n");
        uint8_t high_bits;
        uint8_t low_bits;
        double_reg combined;

        result = mraa_i2c_write_byte(i2c, 0x16);
        result = mraa_i2c_read(i2c, &high_bits, 1);

        result = mraa_i2c_write_byte(i2c, 0x17);
        result = mraa_i2c_read(i2c, &low_bits, 1);

        combined.upper = high_bits;
        combined.lower = low_bits;

        printf("Yaw: High: %d, Low: %d\n", high_bits, low_bits);
        printf("Combined value = 0x%04x, decimal = %f", combined.u_sixteen, (float)combined.u_sixteen/100.0);
        return (float)combined.u_sixteen/100.0;
    }
};

