/*
 * File:   TemplateEventChecker.c
 * Author: Gabriel Hugh Elkaim
 *
 * Template file to set up typical EventCheckers for the  Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the
 * names will have to be changed to match your code.
 *
 * This EventCheckers file will work with both FSM's and HSM's.
 *
 * Remember that EventCheckers should only return TRUE when an event has occured,
 * and that the event is a TRANSITION between two detectable differences. They
 * should also be atomic and run as fast as possible for good results.
 *
 * This file includes a test harness that will run the event detectors listed in the
 * ES_Configure file in the project, and will conditionally compile main if the macro
 * EVENTCHECKER_TEST is defined (either in the project or in the file). This will allow
 * you to check you event detectors in their own project, and then leave them untouched
 * for your project unless you need to alter their post functions.
 *
 * Created on September 27, 2013, 8:37 AM
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

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

#include <xc.h>
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define BATTERY_DISCONNECT_THRESHOLD 175

#define LRSWITCH PORTZ11_BIT

/*******************************************************************************
 * EVENTCHECKER_TEST SPECIFIC CODE                                                             *
 ******************************************************************************/
#define EVENTCHECKER_TEST
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this EventChecker. They should be functions
   relevant to the behavior of this particular event checker */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/

/* Any private module level variable that you might need for keeping track of
   events would be placed here. Private variables should be STATIC so that they
   are limited in scope to this module. */

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

uint8_t CheckDigitalTape(void){
    static ES_EventTyp_t lastEvent = ES_NO_EVENT;
    ES_EventTyp_t curEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    
    unsigned char TapeVal;
    static unsigned char PrevTapeVal;
    
    TapeVal =  Read_DigitalTape();
    
    if ((TapeVal & 0b10) != (PrevTapeVal & 0b10)){
        if (TapeVal & 0b10){
            curEvent = FRONT_TAPE_TRIPPED;
        } else {
             curEvent = FRONT_TAPE_UNTRIPPED;
        }
    }
    
    if ((TapeVal & 0b1) != (PrevTapeVal & 0b1)){
        if (TapeVal & 0b1){
            curEvent = BACK_TAPE_TRIPPED;
        } else {
             curEvent = BACK_TAPE_UNTRIPPED;
        }
    }

    if (curEvent != lastEvent){
        thisEvent.EventType = curEvent;
        returnVal = TRUE;
        lastEvent = curEvent;  
        PostTESTEventService(thisEvent);
    }
    
    PrevTapeVal = TapeVal;
    return returnVal;
}

unsigned char CheckAnalogTape(void){
    
    static ES_EventTyp_t lastEvent_L = ES_NO_EVENT;
    static ES_EventTyp_t lastEvent_R = ES_NO_EVENT;
    ES_EventTyp_t curEvent_L;
    ES_EventTyp_t curEvent_R;
    
    
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    
    uint16_t AnalogTapeReadRight;
    uint16_t AnalogTapeReadLeft;

    AnalogTapeReadRight =  Analog_TapeRead_R();    
    AnalogTapeReadLeft = Analog_TapeRead_L();
    
    if (AnalogTapeReadRight < 250){
        curEvent_R = RIGHT_WALL_INRANGE;    
    } else if (350 < AnalogTapeReadRight < 450){
        curEvent_R = RIGHT_WALL_FAR;
    } else {
        curEvent_R = lastEvent_R;
    }
    
    if (curEvent_R != lastEvent_R){
        lastEvent_R = curEvent_R;
        thisEvent.EventType = curEvent_R;
        PostTESTEventService(thisEvent);
        returnVal = TRUE;
    }

    if (AnalogTapeReadLeft < 250){
        curEvent_L = LEFT_WALL_INRANGE;       
    } else if (AnalogTapeReadLeft > 800){
        curEvent_L = LEFT_WALL_FAR;
    } else {
        curEvent_L = lastEvent_L;
    }
    
    if (curEvent_L != lastEvent_L){
        lastEvent_L = curEvent_L;
        thisEvent.EventType = curEvent_L;
        PostTESTEventService(thisEvent);
        returnVal = TRUE;
    }   
}

uint8_t CheckBumpers(void){
    
    uint8_t Bumper_Curr_Reading = BumperRead();
    static unsigned char Bumper_Prev_Reading;
    
    uint8_t returnVal = FALSE;
    
    static unsigned int i;
    static unsigned char Bumper_Curr_Event;
    static unsigned char Bumper_Prev_Event = 0;

    if (Bumper_Curr_Reading == Bumper_Prev_Reading){
        i++;        
    } else {
        i = 0;
    }
    
    if (i >= 6) {
        Bumper_Curr_Event = Bumper_Curr_Reading;
    }
    
    if (Bumper_Curr_Event != Bumper_Prev_Event){
        ES_Event thisEvent;
        thisEvent.EventType = BUMPER_BUMPED;
        thisEvent.EventParam = Bumper_Curr_Event;
        Bumper_Prev_Event = Bumper_Curr_Event;
        PostTESTEventService(thisEvent);
        returnVal = TRUE;
    }
    
    Bumper_Prev_Reading = Bumper_Curr_Reading;
    return returnVal;    
}

unsigned char CheckTrackWire(void){
    
    static ES_EventTyp_t lastEvent = OFF_WIRE;
    ES_EventTyp_t curEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    static unsigned int i;
    
    unsigned char Reading = ReadTrackWire();
    
    if (Reading){
        i++;
        if (i == 4){
            curEvent = ON_WIRE;
        } else {
            curEvent = OFF_WIRE;
        }
    } else {
        i = 0;
        curEvent = OFF_WIRE;
    }
    
    if (curEvent != lastEvent){
        thisEvent.EventType = curEvent;
        returnVal = TRUE;
        lastEvent = curEvent;  
        PostTESTEventService(thisEvent);
    }
    
    return returnVal;    
}

unsigned char CheckBeacon(void){
    
    static ES_EventTyp_t lastEvent = ES_NO_EVENT;
    ES_EventTyp_t curEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    
    unsigned char Reading = ReadBeacon();
    
    if (Reading){
        curEvent = BEACON_PRESENT;
    } else {
        curEvent = BEACON_ABSENT;
    }
    
    if (curEvent != lastEvent){
        thisEvent.EventType = curEvent;
        returnVal = TRUE;
        lastEvent = curEvent;  
        PostTESTEventService(thisEvent);
    }
    
    return returnVal;    
}

unsigned char CheckSide(void){
    return LRSWITCH;
}
/**
 * @Function TemplateCheckBattery(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function is a prototype event checker that checks the battery voltage
 *        against a fixed threshold (#defined in the .c file). Note that you need to
 *        keep track of previous history, and that the actual battery voltage is checked
 *        only once at the beginning of the function. The function will post an event
 *        of either BATTERY_CONNECTED or BATTERY_DISCONNECTED if the power switch is turned
 *        on or off with the USB cord plugged into the Uno32. Returns TRUE if there was an 
 *        event, FALSE otherwise.
 * @note Use this code as a template for your other event checkers, and modify as necessary.
 * @author Gabriel H Elkaim, 2013.09.27 09:18
 * @modified Gabriel H Elkaim/Max Dunne, 2016.09.12 20:08 */
uint8_t TemplateCheckBattery(void) {
    static ES_EventTyp_t lastEvent = BATTERY_DISCONNECTED;
    ES_EventTyp_t curEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    uint16_t batVoltage = AD_ReadADPin(BAT_VOLTAGE); // read the battery voltage

    if (batVoltage > BATTERY_DISCONNECT_THRESHOLD) { // is battery connected?
        curEvent = BATTERY_CONNECTED;
    } else {
        curEvent = BATTERY_DISCONNECTED;
    }
    if (curEvent != lastEvent) { // check for change from last time
        thisEvent.EventType = curEvent;
        thisEvent.EventParam = batVoltage;
        returnVal = TRUE;
        lastEvent = curEvent; // update history
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
        PostTESTEventService(thisEvent);
#else
//        SaveEvent(thisEvent);
#endif   
    }
    return (returnVal);
}

/* 
 * The Test Harness for the event checkers is conditionally compiled using
 * the EVENTCHECKER_TEST macro (defined either in the file or at the project level).
 * No other main() can exist within the project.
 * 
 * It requires a valid ES_Configure.h file in the project with the correct events in 
 * the enum, and the correct list of event checkers in the EVENT_CHECK_LIST.
 * 
 * The test harness will run each of your event detectors identically to the way the
 * ES_Framework will call them, and if an event is detected it will print out the function
 * name, event, and event parameter. Use this to test your event checking code and
 * ensure that it is fully functional.
 * 
 * If you are locking up the output, most likely you are generating too many events.
 * Remember that events are detectable changes, not a state in itself.
 * 
 * Once you have fully tested your event checking code, you can leave it in its own
 * project and point to it from your other projects. If the EVENTCHECKER_TEST marco is
 * defined in the project, no changes are necessary for your event checkers to work
 * with your other projects.
 */
#ifdef EVENTCHECKER_TEST
#include <stdio.h>

void main(void) {
    
    ES_Return_t x;
    
    BOARD_Init();
    Digital_TapeInit();
    Analog_TapeInit();
    Bumper_Init();
    Motors_Init();
    
    printf("Welcome to team Bday Cakes event checking harness");
    ES_Initialize();
    
    x = ES_Run();
    if (x != 0){
        printf("Error: %d", x);
    }
      
    while(1);
    
    
}
#endif