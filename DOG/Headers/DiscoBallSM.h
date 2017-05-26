/****************************************************************************
 
  Header file for DiscoBallSM

 ****************************************************************************/

#ifndef DISCO_BALL_SM_H
#define DISCO_BALL_SM_H

#include "ES_Configure.h"
#include "ES_Types.h"
#include "ES_Events.h"

// Public Function Prototypes

typedef enum {SearchingForward, SearchingWaiting, SearchingReverse, IndicatingPaired} DiscoBallState_t;

bool InitDiscoBallSM ( uint8_t Priority );
bool PostDiscoBallSM( ES_Event ThisEvent );
ES_Event RunDiscoBallSM( ES_Event ThisEvent );


#endif /* DiscoBallSM */
