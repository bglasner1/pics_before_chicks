#ifndef FARMER_MASTER_SM_H
#define FARMER_MASTER_SM_H

bool InitFarmerMasterSM(uint8_t Priority);
bool PostFarmerMasterSM(ES_Event ThisEvent);
ES_Event RunFarmerMasterSM(ES_Event ThisEvent);
//test comment

#endif
