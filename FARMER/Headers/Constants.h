#ifndef CONSTANTS_H
#define CONSTANTS_H

// GENERAL
#define ALL_BITS (0xFF << 2)
#define VOLUME_THRESHOLD 3000
#define BAUD_RATE_INT 260
#define BAUD_RATE_FRAC 27
#define RX_DATA_LENGTH 43 //Make the array as long as the longest possible message, then only use the cells you need
#define TX_MESSAGE_LENGTH 43 //Make the array as long as the longest possible message, then only use the cells you need
#define IMU_SLAVE_ADDRESS 0xD0
#define BitsPerNibble 4
#define PI 3.141592
#define BROADCAST 0xff

// DATA HEADERS
#define REQ_2_PAIR  0x01
#define PAIR_ACK 	  0x02
#define ENCR_KEY 	  0x03
#define CTRL			  0x04
#define ENCR_RESET	0x05
#define STATUS			0x00

// TRANSMISSION
#define TX_PREAMBLE_LENGTH 8
#define REQ_2_PAIR_LENGTH 2	// This does not include the checksum
#define ENCR_KEY_LENGTH 33  // This does not include the checksum
#define CTRL_LENGTH	4       // This does not include the checksum
#define PAIR_ACK_LENGTH 1	// This does not include the checksum
#define ENCR_RESET_LENGTH 1	// This does not include the checksum
#define STATUS_LENGTH 14    // This does not include the checksum
#define PACKET_LENGTH_MSB 0x00
#define TX_API_IDENTIFIER 0x01
#define RX_API_IDENTIFIER 0x81
#define START_DELIMITER 0x7E
#define TX_FRAME_ID 0x01 // must be a nonzero value
#define OPTIONS 0x00
#define FRAME_DATA_START 3
#define FRAME_DATA_PREAMBLE_LENGTH 5

// RECEIVE
 #define RX_API_IDENTIFIER 0x81

// DRIVING
#define IDLE 127
#define MAX_REVERSE 0
#define MAX_FORWARD 255

// STEERING
#define STRAIGHT 127
#define MAX_LEFT_TURN 255
#define MAX_RIGHT_TURN 0
 
// TIMING
#define DEBOUNCE_TIME 50
#define CONNECTION_TIME 1000
#define TRANSMISSION_RATE 300
#define I2C_COMM_SPEED 200
#define I2C_DELAY_TIME 2000
#define CALIBRATION_TIME 1000
#define IMU_POLL_TIME 100

// PINS
#define R_BUTTON_B GPIO_PIN_0
#define L_BUTTON_B GPIO_PIN_1
#define REVERSE_BUTTON_D GPIO_PIN_0
#define PERIPHERAL_BUTTON_D GPIO_PIN_1
#define SOUND_PIN_E GPIO_PIN_0
#define NUMBER_OF_ANALOG_PINS 1
#define RX_PIN 4
#define TX_PIN 5
#define RX_ALT_FUNC 2
#define TX_ALT_FUNC 2
#define G_LED_1_B GPIO_PIN_2
#define Y_LED_1_B GPIO_PIN_3
#define G_LED_2_B GPIO_PIN_4
#define Y_LED_2_B GPIO_PIN_5
#define G_LED_3_B GPIO_PIN_6
#define Y_LED_3_B GPIO_PIN_7
#define I2C_SDA_PIN GPIO_PIN_5
#define I2C_SCL_PIN GPIO_PIN_4
#define I2C_SDA_BIT 5
#define I2C_SCL_BIT 4

// Bytes
#define INIT_BYTE 0x7E
#define NUM_XBEE_BYTES 4
#define BITS_PER_NIBBLE 4
#define API_81 0x81

// PERIPHERAL/REGISTER
#define I2C_PIN_M 0xff00ffff
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
#define POWER_REGISTER 0x6B
#define POWER_SETTING 0x01
#define GYROSCOPE_X_REGISTER_BASE 0x44
#define GYROSCOPE_Y_REGISTER_BASE 0x46
#define GYROSCOPE_Z_REGISTER_BASE 0x48
#define ACCELEROMETER_X_REGISTER_BASE 0x3C
#define ACCELEROMETER_Y_REGISTER_BASE 0x3E
#define ACCELEROMETER_Z_REGISTER_BASE 0x40

#endif //CONSTANTS_H//
