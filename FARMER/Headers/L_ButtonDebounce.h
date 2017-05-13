#ifndef L_BUTTONDEBOUNCE_H
#define L_BUTTONDEBOUNCE_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

// typedefs for the states
// State definitions for use with the query function
typedef enum {L_Debouncing, L_Ready2Sample} L_DBState_t;

// Public Function Prototypes

bool InitL_ButtonDebounce(uint8_t Priority);
bool PostL_ButtonDebounce(ES_Event ThisEvent);
ES_Event RunL_ButtonDebounce(ES_Event ThisEvent);


#endif /* L_BUTTONDEBOUNCE_H */
