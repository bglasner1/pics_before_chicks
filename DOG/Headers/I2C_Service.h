#ifndef I2C_SERVICE_H
#define I2C_SERVICE_H

#include "ES_Configure.h"
#include "ES_Types.h"

typedef enum {I2C_Init, I2C_Poll_IMU} I2C_State;

bool Init_I2C(uint8_t Priority);
bool Post_I2C( ES_Event ThisEvent );
ES_Event Run_I2C(ES_Event ThisEvent);
uint8_t getAccelX_MSB(void);
uint8_t getAccelX_LSB(void);
uint8_t getAccelY_MSB(void);
uint8_t getAccelY_LSB(void);
uint8_t getAccelZ_MSB(void);
uint8_t getAccelZ_LSB(void);
uint8_t getGyroX_MSB(void);
uint8_t getGyroX_LSB(void);
uint8_t getGyroY_MSB(void);
uint8_t getGyroY_LSB(void);
uint8_t getGyroZ_MSB(void);
uint8_t getGyroZ_LSB(void);



#endif //I2C_SERVICE_H//
