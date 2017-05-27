#ifndef HARDWARE_H
#define HARDWARE_H

void Hardware_Init(void);
void SetThrustFan(uint8_t DriveCtrl);
void SetDutyThrustFan(uint8_t duty);
void SetDutyIndicator(uint8_t duty);
void SetDirectionThrust(uint8_t dir);
void SetLeftBrakePosition(uint16_t position);
void SetRightBrakePosition(uint16_t position);
uint8_t ReadDOGTag(void);
void sendToPIC(uint8_t value);

void SetDirectionDiscoBall(uint8_t dir);

#endif //HARDWARE_H//
