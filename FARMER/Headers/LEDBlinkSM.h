/****************************************************************************
 
  Header file for LEDBlinkSM

 ****************************************************************************/

#ifndef LED_BLINK_SM_H
#define LED_BLINK_SM_H

#include "ES_Configure.h"
#include "ES_Types.h"

typedef enum {InitPS, BlinkOn, BlinkOff, PairedSolid, } LEDBlinkState_t;


bool InitLEDBlinkSM(uint8_t Priority);
bool PostLEDBlinkSM(ES_Event ThisEvent);
ES_Event RunLEDBlinkSM(ES_Event ThisEvent);


#endif
