#ifndef HARDWARE_H
#define HARDWARE_H

#include "ES_Configure.h"
#include "ES_Types.h"


void Hardware_Init(void);
void SetDutyThrustFan(uint8_t duty);
void SetDutyIndicator(uint8_t duty);
void SetDirectionThrust(uint8_t dir);
void SetLeftBrakePosition(uint16_t position);
void SetRightBrakePosition(uint16_t position);
uint8_t ReadDOGTag(void);


#endif //HARDWARE_H//
