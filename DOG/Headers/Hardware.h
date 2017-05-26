#ifndef HARDWARE_H
#define HARDWARE_H

void Hardware_Init(void);
void SetDutyThrustFan(uint8_t duty);
void SetDutyIndicator(uint8_t duty);
void SetDutyRightVibrationMotor(uint8_t duty);
void SetDutyLeftVibrationMotor(uint8_t duty);
void SetDirectionThrust(uint8_t dir);
void SetLeftBrakePosition(uint16_t position);
void SetRightBrakePosition(uint16_t position);
uint8_t ReadDOGTag(void);


#endif //HARDWARE_H//
