
#ifndef HARDWARE_TEST_SERVICE_H
#define HARDWARE_TEST_SERVICE_H

#include "ES_Configure.h"
#include "ES_Types.h"

bool InitHardwareTestService(uint8_t Priority);
bool PostHardwareTestService(ES_Event ThisEvent);
ES_Event RunHardwareTestService(ES_Event ThisEvent);


#endif
