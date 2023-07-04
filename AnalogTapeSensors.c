#include "DigitalTapeSensors.h"
#include "ES_Configure.h"
#include "BCEventChecker.h"
#include "ES_Framework.h"
#include "ES_Events.h"

#include "BOARD.h"
#include "serial.h"
#include "AD.h"
#include "IO_Ports.h"
#include "pwm.h"

#include <xc.h>
#include <stdio.h>

#define LEFTSENSOR AD_PORTW3
#define RIGHTSENSOR AD_PORTW4

unsigned char Analog_TapeInit(void){
    
    AD_Init();
    return AD_AddPins(LEFTSENSOR | RIGHTSENSOR);   
    
}

uint16_t Analog_TapeRead_L(void){
    
    return AD_ReadADPin(LEFTSENSOR);
    
}

uint16_t Analog_TapeRead_R(void){
    
    return AD_ReadADPin(RIGHTSENSOR);
    
}