#include <stdlib.h>
#include <cmath>
#include "include/swerve_module.h"
#include "include/drive_module.h"
#include "include/pot_module.h"

int main(int argc, char** argv) {
    uint8_t address = 0x40;
    mraa_i2c_context i2c = mraa_i2c_init(6);
    mraa_result_t i2c_status = i2c_init_board(i2c, address);
    if (i2c_status != MRAA_SUCCESS) printf("[ !!! ] Can not initialize I2C Board.\n");

    int motor = 0; // default motor chosen
    char direction = 'X'; // default direction
    int PWM = 0;
    int swerve_port = 6;
    int drive_port = 7;

    // set motor and ports
    if (argc > 1) {
        if (atoi(argv[1]) == 0) {
            printf("FRONT RIGHT motor chosen, %s\n", argv[1]);
            motor = 0;
            swerve_port = 6;
            drive_port = 7;
        } else if (atoi(argv[1]) == 1) {
            printf("BACK RIGHT motor chosen, %s\n", argv[1]);
            motor = 1;
            swerve_port = 4;
            drive_port = 5;
        } else if (atoi(argv[1]) == 2) {
            printf("BACK LEFT motor chosen, %s\n", argv[1]);
            motor = 2;
            swerve_port = 0;
            drive_port = 1;
        } else if (atoi(argv[1]) == 3) {
            printf("FRONT LEFT motor chosen, %s\n", argv[1]);
            motor = 3;
            swerve_port = 2;
            drive_port = 3;
        }
    }
    if (argc == 4) {
        printf("CUSTOM POT VALUE: %d\n", atoi(argv[3]));
        direction = 'X';
        PWM = atoi(argv[3]);
    } else {
        if (argc == 3) {
            direction = argv[2][0];
            if (argv[2][0] == 'X') {
                printf("X direction chosen\n");
            } else if (argv[2][0] == 'Y') {
                printf("Y direction chosen\n");
            } else if (argv[2][0] == 'Z') {
                printf("Z direction chosen\n");
            }
        }
    }
    Swerve_Module s1 = Swerve_Module(i2c, 0, swerve_port, drive_port, motor, 0, 2508, 1992, 2127);

    // set positions
    if (PWM != 0) {
        s1.x_pos = PWM;
        direction = 'X';
        printf("GOING TO CUSTOM VALUE OF %d\n", PWM);
    } else {
        if(motor == 0) {
            // FRONT RIGHT
            s1.x_pos = 2508;
            s1.y_pos = 1992;
            s1.z_pos = 2127;
        } else if (motor == 1) {
            // BACK RIGHT
            s1.x_pos = 1480;
            s1.y_pos = 1921;
            s1.z_pos = 1810;
        } else if (motor == 2) {
            // BACK LEFT
            s1.x_pos = 2540;
            s1.y_pos = 2040;
            s1.z_pos = 2190;
        } else if (motor == 3) {
            // FRONT LEFT
            s1.x_pos = 1753;
            s1.y_pos = 2266;
            s1.z_pos = 2089;
        }
    }

    bool waiting = false;
    int swerve_result1;
    while(1) {
        usleep(50000); //sleep for 0.05s
        swerve_result1 = s1.swerve_controller(direction, 0.6, false, waiting);
        if(swerve_result1 == 0) {
            printf("ALIGNED!\n");
            waiting = true;
            swerve_result1 = s1.swerve_controller(direction, 0.6, false, waiting );
        } else {
            if(PWM) {
                printf("ROTATING TO = %d CURRENT POS = %d\n", PWM, s1.current_pos);
            } else {
                printf("ROTATING TO DEFAULT POS = %c CURRENT POS = %d\n", direction, s1.current_pos);
            }
        }
        usleep(50);
        mraa_i2c_write_byte_data(i2c, ((uint8_t) 0xa0), ((uint8_t) 0x00)); // wake up the board
    }

    return 0;
}