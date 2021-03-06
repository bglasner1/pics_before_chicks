#ifndef REV_BUTTONDEBOUNCE_H
#define REV_BUTTONDEBOUNCE_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

// typedefs for the states
// State definitions for use with the query function
typedef enum {REV_Debouncing, REV_Ready2Sample} REV_DBState_t;

// Public Function Prototypes

bool InitREV_ButtonDebounce(uint8_t Priority);
bool PostREV_ButtonDebounce(ES_Event ThisEvent);
ES_Event RunREV_ButtonDebounce(ES_Event ThisEvent);


#endif /* REV_BUTTONDEBOUNCE_H */
