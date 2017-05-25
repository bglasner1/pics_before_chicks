
#ifndef HARDWARE_TEST_SERVICE_H
#define HARDWARE_TEST_SERVICE_H



bool InitHardwareTestService(uint8_t Priority);
bool PostHardwareTestService(ES_Event ThisEvent);
ES_Event RunHardwareTestService(ES_Event ThisEvent);

#endif
