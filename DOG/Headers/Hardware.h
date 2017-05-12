#ifndef HARDWARE_H
#def HARDWARE_H

void Hardware_Init(void);
void SetDutyThrustFan(uint8_t duty);
void SetDutyIndicator(uint8_t duty);
void SetDirectionMotor(uint8_t dir);
void SetLeftBrakePosition(uint16_t position);
void SetRightBrakePosition(uint16_t position);


#endif //HARDWARE_H//