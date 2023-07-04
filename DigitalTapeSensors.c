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

#define TAPESENSOR_F PORTX03_BIT 
#define TAPESENSOR_B PORTX04_BIT 

unsigned char Digital_TapeInit(void){
    
    return IO_PortsSetPortInputs(PORTX , PIN3 | PIN4);
    
}

unsigned char Read_DigitalTape(void){
    
    return (TAPESENSOR_F << 1) | (TAPESENSOR_B); 
    
}