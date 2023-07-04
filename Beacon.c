#include "ES_Configure.h"
#include "BCEventChecker.h"
#include "BumperSensor.h"
#include "ES_Framework.h"
#include "ES_Events.h"

#include "BOARD.h"
#include "serial.h"
#include "AD.h"
#include "IO_Ports.h"
#include "pwm.h"
#include "Beacon.h"

#include <xc.h>
#include <stdio.h>

#define BEACON AD_PORTW8

unsigned char Beacon_Init(void){
    AD_Init();
    return AD_AddPins(BEACON);
    
}

unsigned int ReadBeacon(void){
    //printf("Beacon Reading: %d \r\n", AD_ReadADPin(BEACON));
    return AD_ReadADPin(BEACON);    
    
}
