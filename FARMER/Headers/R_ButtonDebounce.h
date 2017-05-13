#ifndef R_BUTTONDEBOUNCE_H
#define R_BUTTONDEBOUNCE_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

// typedefs for the states
// State definitions for use with the query function
typedef enum {R_Debouncing, R_Ready2Sample} R_DBState_t;

// Public Function Prototypes

bool InitR_ButtonDebounce(uint8_t Priority);
bool PostR_ButtonDebounce(ES_Event ThisEvent);
ES_Event RunR_ButtonDebounce(ES_Event ThisEvent);


#endif /* R_BUTTONDEBOUNCE_H */
