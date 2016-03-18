#include "mraa/aio.h"
#include "i2c_library.h"

#ifndef MOTOR_MODULE_H
#define MOTOR_MODULE_H

class Motor_Module {
// move this into main function
uint8_t end_reg;

public:
    Motor_Module() {
        // start reg begins at port 1 by default
        end_reg = 0x0C;
    }

    Motor_Module(uint8_t reg) {
        end_reg = 0x0C + (reg - 1)*4;
        printf("Motor module initialized to port 1 (register 0x%02x\n", end_reg);
    }

    mraa_result_t send_signal(mraa_i2c_context i2c_context, double_reg signal) {
        mraa_result_t result = MRAA_SUCCESS;
        result = i2c_send_signal(i2c_context, end_reg, signal);
        if(result != MRAA_SUCCESS) {
            printf("Could not write to register 0x%02x\n", end_reg);
            return result;
        }
        return result;
    }

};

#endif
