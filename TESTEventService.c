/*
 * File: TemplateService.h
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a simple service to work with the Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that this file
 * will need to be modified to fit your exact needs, and most of the names will have
 * to be changed to match your code.
 *
 * This is provided as an example and a good place to start.
 *
 * Created on 23/Oct/2011
 * Updated on 13/Nov/2013
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "BOARD.h"
#include "AD.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "TESTEventService.h"
#include "BCEventChecker.h"
#include "Motor_Driver.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

#define BATTERY_DISCONNECT_THRESHOLD 175
#define TIMER_0_TICKS 50
#define TAPE_SERVICE_TIMER 0
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/
/* You will need MyPriority and maybe a state variable; you may need others
 * as well. */

static uint8_t MyPriority;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateService(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateService function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitTESTEventService(uint8_t Priority)
{
    ES_Event ThisEvent;

    MyPriority = Priority;
    // in here you write your initialization code
    // this includes all hardware and software initialization
    // that needs to occur.
    // post the initial transition event
    ES_Timer_InitTimer(TAPE_SERVICE_TIMER, TIMER_0_TICKS);
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostTESTEventService(ES_Event ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the service,
 *        as this is called any time a new event is passed to the event queue. 
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed." 
 * @author J. Edward Carryer, 2011.10.23 19:25 */
ES_Event RunTESTEventService(ES_Event ThisEvent)
{
    ES_Event ReturnEvent;
    
    //printf("event type : %d", ThisEvent.EventType);
    switch (ThisEvent.EventType){
        case (ES_TIMEOUT):
            CheckDigitalTape();
            CheckAnalogTape();
            CheckBumpers();
            CheckTrackWire();
            CheckBeacon();
            ES_Timer_InitTimer(TAPE_SERVICE_TIMER, TIMER_0_TICKS);
            break;
        case (FRONT_TAPE_TRIPPED):
            LeftWheelSpeed(0);
            RightWheelSpeed(0);
            printf("FRONT TAPE SENSOR TRIPPED.....");
            break;
        
        case (FRONT_TAPE_UNTRIPPED):
            printf("FRONT TAPE SENSOR (UN)TRIPPED.....");
            break;
            
        case (BACK_TAPE_TRIPPED):
            printf("BACK TAPE SENSOR TRIPPED.....");
            
            
            
            break;
        
        case (BACK_TAPE_UNTRIPPED):
            printf("BACK TAPE SENSOR (UN)TRIPPED.....");
            break;
            
        case (LEFT_WALL_INRANGE):
            printf("LEFT WALL IN RANGE.........");
           ///LeftWheelSpeed(1000);
            //RightWheelSpeed(1000);
            break;
            
        case (LEFT_WALL_FAR):
            printf("LEFT WALL FAR.........");
            //LeftWheelSpeed(0);
            //RightWheelSpeed(0);
            break;    
            
            
        case (RIGHT_WALL_INRANGE):
            printf("RIGHT WALL IN RANGE.........");
           // LeftWheelSpeed(-1000);
           // RightWheelSpeed(-1000);
            break;
            
        case (RIGHT_WALL_FAR):
            printf("RIGHT WALL FAR.........");
            //LeftWheelSpeed(0);
            //RightWheelSpeed(0);
            break;   
            
        case (BUMPER_BUMPED):
            printf("Bumper Bumped: %d", ThisEvent.EventParam);
            
            if (ThisEvent.EventParam == 1){
                LeftWheelSpeed(0);
                RightWheelSpeed(0);
            } else if (ThisEvent.EventParam == 2){
                LeftWheelSpeed(0);
                RightWheelSpeed(0);
            } else if (ThisEvent.EventParam == 4){
                LeftWheelSpeed(1000);
                RightWheelSpeed(1000);
            } else if (ThisEvent.EventParam == 8){
                LeftWheelSpeed(-1000);
                RightWheelSpeed(-1000);
            }
            break;
            
        case (ON_WIRE) :
            printf("ON TRACKWIRE.....");
            break;
        
        case (OFF_WIRE):
            printf("OFF TRACKWIRE....");
            break;
            
        case (BEACON_PRESENT):
            printf("BEACON DETECTED.......");
            break;
            
        case (BEACON_ABSENT) :
            printf("BEACON ABSENT.......");
            break;
            
        default:
            break;
    }
    ReturnEvent.EventType = ES_NO_EVENT;      
    
    return ReturnEvent;
}