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
#include "TrackWire.h"

#include <xc.h>
#include <stdio.h>

#define TRACKWIRE PORTZ03_BIT

unsigned char TrackWire_Init(void){
    
    return IO_PortsSetPortInputs(PORTZ, PIN3);
    
}

unsigned char ReadTrackWire(void){
    
    return TRACKWIRE;    
    
}
