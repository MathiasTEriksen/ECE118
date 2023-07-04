#include "ES_Configure.h"   // defines ES_Event, INIT_EVENT, ENTRY_EVENT, and EXIT_EVENT

uint8_t InitTESTEventService(uint8_t Priority);

uint8_t PostTESTEventService(ES_Event ThisEvent);

ES_Event RunTESTEventService(ES_Event ThisEvent);

