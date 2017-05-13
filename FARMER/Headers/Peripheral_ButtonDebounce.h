#ifndef P_BUTTONDEBOUNCE_H
#define P_BUTTONDEBOUNCE_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

// typedefs for the states
// State definitions for use with the query function
typedef enum {P_Debouncing, P_Ready2Sample} P_DBState_t;

// Public Function Prototypes

bool InitP_ButtonDebounce(uint8_t Priority);
bool PostP_ButtonDebounce(ES_Event ThisEvent);
ES_Event RunP_ButtonDebounce(ES_Event ThisEvent);


#endif /* P_BUTTONDEBOUNCE_H */
