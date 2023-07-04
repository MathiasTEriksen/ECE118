#include "BOARD.h"
#include <xc.h>
#include <stdio.h>
#include "ES_Configure.h"
#include "BCEventChecker.h"
#include "ES_Events.h"
#include "ES_Framework.h"
#include "serial.h"
#include "DigitalTapeSensors.h"
#include "AnalogTapeSensors.h"
#include "BumperSensor.h"
#include "TrackWire.h"
#include "Beacon.h"
#include "AD.h"
#include "IO_Ports.h"
#include "Motor_Driver.h"

#include "BdayFSM.h"
#include "Servo.h"


void main(void)
{
    int i;
    ES_Return_t ErrorType;

    BOARD_Init();
    TurnStile_Init();
    Stop_Ball();
    Digital_TapeInit();
    Analog_TapeInit();
    Bumper_Init();
    Motors_Init();
    
    for (i = 0; i<20000;i++) {
        asm("NOP");
    }
    
    printf("Starting ES Framework Template\r\n");
    printf("using the 2nd Generation Events & Services Framework\r\n");


    // Your hardware initialization function calls go here

    // now initialize the Events and Services Framework and start it running
    ErrorType = ES_Initialize();
    if (ErrorType == Success) {
        ErrorType = ES_Run();

    }
    //if we got to here, there was an error
    switch (ErrorType) {
    case FailedPointer:
        printf("Failed on NULL pointer");
        break;
    case FailedInit:
        printf("Failed Initialization");
        break;
    default:
        printf("Other Failure: %d", ErrorType);
        break;
    }

    while(1);
};

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
