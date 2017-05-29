#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "driverlib/i2c.h"
#include "inc/hw_i2c.h"

// GENERAL
#define FORWARD 0
#define REVERSE 1
#define DOG_3_THRESHOLD 1100
#define DOG_2_THRESHOLD 1400
#define DOG_1 1
#define DOG_2 2
#define DOG_3 3
#define BAUD_RATE_INT 260
#define BAUD_RATE_FRAC 27
#define PIC_BAUD_RATE_INT 253
#define PIC_BAUD_RATE_FRAC 9
#define RX_MESSAGE_LENGTH 43
#define RX_DATA_OFFSET 8
#define RX_DATA_LENGTH 43
#define TX_MESSAGE_LENGTH 22
#define IMU_SLAVE_ADDRESS 0xD6
#define BitsPerNibble 4
#define PI 3.141592
#define ENCR_LENGTH 32
#define DATA_MIDPOINT 127
#define MAX_DATA_LENGTH 4

// TRANSMISSION
#define TX_PREAMBLE_LENGTH 8
#define REQ_2_PAIR_LENGTH 2	// This does not include the checksum
#define ENCR_KEY_LENGTH 33  // This does not include the checksum
#define CTRL_LENGTH	4       // This does not include the checksum
#define PAIR_ACK_LENGTH 1	// This does not include the checksum
#define ENCR_RESET_LENGTH 1	// This does not include the checksum
#define STATUS_LENGTH 13    // This does not include the checksum
#define PACKET_LENGTH_MSB 0x00
#define TX_API_IDENTIFIER 0x01
#define RX_API_IDENTIFIER 0x81
#define START_DELIMITER 0x7E
#define TX_FRAME_ID 0x01 // must be a nonzero value
#define OPTIONS 0x00
#define FRAME_DATA_START 3
#define FRAME_DATA_PREAMBLE_LENGTH 5

// Bytes
#define INIT_BYTE 0x7E
#define NUM_XBEE_BYTES 4
#define BITS_PER_NIBBLE 4

//Masks
#define BRAKE_MASK 0x02
#define PER_MASK 0x01
#define BROAD_MASK 0x02
#define API_81 0x81
#define REQ_2_PAIR 0x01
#define PAIR_ACK 0x02
#define ENCR_KEY 0x03
#define CTRL 0x04
#define ENCR_RESET 0x05
#define STATUS 0x00


// TIMING
#define TicksPerMS 40000
#define PWMTicksPerMS TicksPerMS/32
#define MotorFreqHz 2000
#define ServoFreqHz 50
#define MOTOR_PWM_PERIOD PWMTicksPerMS*1000/MotorFreqHz
#define SERVO_PWM_PERIOD PWMTicksPerMS*1000/ServoFreqHz
#define LEFT_SERVO_IDLE_DUTY (SERVO_PWM_PERIOD >> 2)*(5/100)
#define RIGHT_SERVO_IDLE_DUTY (SERVO_PWM_PERIOD >> 2)*(5/100)
#define CONNECTION_TIME 1000
#define TRANSMISSION_RATE 200
#define I2C_COMM_SPEED 200
#define I2C_DELAY_TIME 3000
#define CALIBRATION_TIME 1000
#define IMU_POLL_TIME 100

//PIC
#define LIFT_FAN_ON 25
#define LIFT_FAN_OFF 0

//ELECTROMECHANICAL INDICATOR
#define DISCO_SPIN_TIME 500
#define DISCO_WAIT_TIME 500
#define DISCO_FORWARD_DUTY 80
#define DISCO_REVERSE_DUTY 80
#define DISCO_DUTY_OFF 0


// PINS
#define THRUST_FAN_DIR_B GPIO_PIN_0
#define DOG_TAG_E GPIO_PIN_0
#define NUMBER_OF_ANALOG_PINS 1
#define LEFT_SERVO_PIN_B GPIO_PIN_4
#define LEFT_SERVO_BIT 4
#define RIGHT_SERVO_PIN_B GPIO_PIN_5
#define RIGHT_SERVO_BIT 5
#define THRUST_FAN_PWM_PIN_B GPIO_PIN_6
#define THRUST_FAN_PWM_BIT 6
#define INDICATOR_PIN_B GPIO_PIN_7
#define INDICATOR_BIT 7
#define RX_PIN 4
#define TX_PIN 5
#define TX_PIC_PIN 7
#define RX_ALT_FUNC 2
#define TX_ALT_FUNC 2
#define TX_PIC_ALT_FUNC 1
#define I2C_SDA_PIN GPIO_PIN_5
#define I2C_SCL_PIN GPIO_PIN_4
#define I2C_SDA_BIT 5
#define I2C_SCL_BIT 4
#define TX_PIC_PIN 7
#define RIGHT_VIBRATION_MOTOR_F GPIO_PIN_2
#define LEFT_VIBRATION_MOTOR_F GPIO_PIN_3
#define RIGHT_VIBRATION_MOTOR_BIT 2
#define LEFT_VIBRATION_MOTOR_BIT 3

// PERIPHERAL/REGISTER
#define ALL_BITS (0xFF << 2)
#define PWM_PIN_M_B 0x0000ffff
#define PWM_PIN_M_F 0xffff00ff
#define I2C_PIN_M 0xff00ffff
#define GenA_0_Normal (PWM_0_GENA_ACTCMPAU_ONE | PWM_0_GENA_ACTCMPAD_ZERO)
#define GenB_0_Normal (PWM_0_GENB_ACTCMPBU_ONE | PWM_0_GENB_ACTCMPBD_ZERO)
#define GenA_1_Normal (PWM_1_GENA_ACTCMPAU_ONE | PWM_1_GENA_ACTCMPAD_ZERO)
#define GenB_1_Normal (PWM_1_GENB_ACTCMPBU_ONE | PWM_1_GENB_ACTCMPBD_ZERO)
#define GenA_0_Invert (PWM_0_GENA_ACTCMPAU_ZERO | PWM_0_GENA_ACTCMPAD_ONE)
#define GenB_0_Invert (PWM_0_GENB_ACTCMPBU_ZERO | PWM_0_GENB_ACTCMPBD_ONE)
#define I2C_MCS_WRITE_M 0xFFFFFFE0
#define I2C_MCS_SINGLE_TX (I2C_MCS_START | I2C_MCS_STOP | I2C_MCS_RUN)
#define I2C_MCS_LAST_TX (I2C_MCS_STOP | I2C_MCS_RUN)
#define I2C_MCS_START_TX (I2C_MCS_START | I2C_MCS_RUN)
#define I2C_MCS_CONTINUE_TX I2C_MCS_RUN
#define I2C_MCS_SINGLE_RX (I2C_MCS_START | I2C_MCS_STOP | I2C_MCS_RUN)
#define I2C_MCS_START_RX (I2C_MCS_ACK | I2C_MCS_START | I2C_MCS_RUN)
#define I2C_MCS_CONTINUE_RX (I2C_MCS_ACK | I2C_MCS_RUN)
#define I2C_MCS_LAST_RX (I2C_MCS_STOP | I2C_MCS_RUN)

// IMU Registers/Data
#define GYROSCOPE_POWER_REGISTER 0x11
#define GYROSCOPE_POWER_SETTING 0x70
#define ACCELEROMETER_POWER_REGISTER 0x10
#define ACCELEROMETER_POWER_SETTING 0x70
#define GYROSCOPE_X_REGISTER_BASE 0x22
#define GYROSCOPE_Y_REGISTER_BASE 0x24
#define GYROSCOPE_Z_REGISTER_BASE 0x26
#define ACCELEROMETER_X_REGISTER_BASE 0x28
#define ACCELEROMETER_Y_REGISTER_BASE 0x2A
#define ACCELEROMETER_Z_REGISTER_BASE 0x2C



#endif //CONSTANTS_H//
