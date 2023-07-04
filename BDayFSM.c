/*
 * File: TemplateFSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Flat State Machine to work with the Events and Services
 * Frameword (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that this file
 * will need to be modified to fit your exact needs, and most of the names will have
 * to be changed to match your code.
 *
 * This is provided as an example and a good place to start.
 *
 *Generally you will just be modifying the statenames and the run function
 *However make sure you do a find and replace to convert every instance of
 *  "Template" to your current state machine's name
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 09/13/13 15:17 ghe      added tattletail functionality and recursive calls
 * 01/15/12 11:12 jec      revisions for Gen2 framework
 * 11/07/11 11:26 jec      made the queue static
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 * 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
 */


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"

#include "BdayFSM.h"
#include "BOARD.h"
#include "BCEventChecker.h"
#include "Motor_Driver.h"
#include "AD.h"
#include "pwm.h"
#include "RC_Servo.h"
#include "DigitalTapeSensors.h"
#include "AnalogTapeSensors.h"
#include "BumperSensor.h"
#include "TrackWire.h"
#include "Beacon.h"
#include "Servo.h"
#include "OnePointerSubHSM.h"
#include "TwoPointerSubHSM.h"
#include "ThreePointerSubHSM.h"

#include <stdio.h>


/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

#define BATTERY_DISCONNECT_THRESHOLD 175

#define TIMER_0_TICKS 50
#define TAPE_SERVICE_TIMER 0
 
#define MOVE_FWD_TIMER 1
#define MOVE_FWD_TICKS 500

#define RELOAD_TIMER 6
#define RELOAD_TICKS 3000
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine.*/


/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/

/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

typedef enum {
    Init,
    Find_Beacon,
    Find_Wall,
    Align_Wall,
    Follow_Wall,
    Shoot_1PT,
    Go_To_2PT,        
    Shoot_2PT,
    Go_To_3PT,        
    Shoot_3PT,
    Reload
} BdayFSMState_t;

static const char *StateNames[] = {
	"Init",
    "Find_Beacon",
    "Find_Wall",
    "Align_Wall",
    "Follow_Wall",
    "Shoot_1PT",
    "Shoot_2PT",
    "Shoot_3PT",
    "Reload"
};


static BdayFSMState_t CurrentState = Init; // <- change enum name to match ENUM
static uint8_t MyPriority;

static unsigned char Side;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateFSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitBdayFSM(uint8_t Priority)
{
    MyPriority = Priority;
    // put us into the Initial PseudoState
    CurrentState = Init;
    
    AD_Init();
    PWM_Init();
    RC_Init();
    
    Digital_TapeInit();
    Analog_TapeInit();
    Bumper_Init();
    Motors_Init();
    
    TurnStile_Init();
    
    ES_Timer_InitTimer(TAPE_SERVICE_TIMER, TIMER_0_TICKS);
    
    Side = CheckSide();
    Stop_Ball();
    
    // post the initial transition event
    if (ES_PostToService(MyPriority, INIT_EVENT) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostTemplateFSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostBdayFSM(ES_Event ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateFSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the flat state machine,
 *        as this is called any time a new event is passed to the event queue. This
 *        function will be called recursively to implement the correct order for a
 *        state transition to be: exit current state -> enter next state using the
 *        ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed."
 * @author J. Edward Carryer, 2011.10.23 19:25 */
ES_Event RunTemplateFSM(ES_Event ThisEvent)
{
    
    switch (ThisEvent.EventType){
        
        case (ES_TIMEOUT):
            if (ThisEvent.EventParam == TAPE_SERVICE_TIMER){
                CheckDigitalTape();
                CheckAnalogTape();
                CheckBumpers();
                CheckTrackWire();
                CheckBeacon();
                ES_Timer_InitTimer(TAPE_SERVICE_TIMER, TIMER_0_TICKS);
            }
            break;
        case(ES_TIMERACTIVE):
            break;
        case(ES_TIMERSTOPPED):
            break;
        default:
            break;
    }
    
    switch (CurrentState) {
        
        case Init: // If current state is initial Psedudo State
            CurrentState = Find_Beacon;
            break;

        case Find_Beacon: // in the first state, replace this with appropriate state

            LeftWheelSpeed(-1000);
            RightWheelSpeed(1000);

            if (ThisEvent.EventType == BEACON_PRESENT){
                CurrentState = Find_Wall;
            }
            break;
        
        case Find_Wall:
            if (Side == RIGHT){
                
                LeftWheelSpeed(500);
                RightWheelSpeed(250);
                
                if (ThisEvent.EventType = BUMPER_BUMPED){
                    if (ThisEvent.EventParam == 4) {
                            CurrentState = Align_Wall;
                    }
                }
                
            } else if (Side == LEFT){
                
                LeftWheelSpeed(250);
                RightWheelSpeed(500);
                
                if (ThisEvent.EventType = BUMPER_BUMPED){
                    if (ThisEvent.EventParam == 8) {
                            CurrentState = Align_Wall;
                    }
                }
            }
            break;
            
        case Align_Wall:
            
            if (Side == RIGHT){
                
                LeftWheelSpeed(250);
                RightWheelSpeed(500);
                
                if (ThisEvent.EventType == RIGHT_WALL_INRANGE){
                    CurrentState = Follow_Wall;
                }
            }
            
            else if (Side == LEFT){
                
                LeftWheelSpeed(500);
                RightWheelSpeed(250);
                
                if (ThisEvent.EventType == LEFT_WALL_INRANGE){
                    CurrentState = Follow_Wall;
                }
            }
            break;
        
        case Follow_Wall:
            
            RightWheelSpeed(1000);
            LeftWheelSpeed(1000);
            
            if (BACK_TAPE_TRIPPED){
                CurrentState = Shoot_1PT;
                ES_Timer_InitTimer(MOVE_FWD_TIMER, MOVE_FWD_TICKS);
            } 
            break;
            
        case Shoot_1PT:                        
            RunOnePointerSubHSM(ThisEvent);
            RightWheelSpeed(1000);
            LeftWheelSpeed(1000);
            
            if (ThisEvent.EventType == SHOOTING_1PT_DONE){
                CurrentState = Go_To_2PT;
            }
            break;
            
        case Go_To_2PT:
            
            RightWheelSpeed(-1000);
            LeftWheelSpeed(-1000);
                
            if (ThisEvent.EventType = FRONT_TAPE_TRIPPED){
                CurrentState = Shoot_2PT;
                ES_Timer_InitTimer(MOVE_FWD_TIMER, MOVE_FWD_TICKS);
            }
            break;
            
        case Shoot_2PT:
            
            RunTwoPointerSubHSM(ThisEvent);
            RightWheelSpeed(0);
            LeftWheelSpeed(0);
            
            if (ThisEvent.EventType == SHOOTING_2PT_DONE){
                CurrentState = Go_To_3PT;
            }
            
        case Go_To_3PT:
            
            RightWheelSpeed(-1000);
            LeftWheelSpeed(-1000);
                
            if (ThisEvent.EventType = ON_WIRE){
                CurrentState = Shoot_3PT;
                ES_Timer_InitTimer(MOVE_FWD_TIMER, MOVE_FWD_TICKS);
            }
            break;
            
        case Shoot_3PT:
            RunThreePointerSubHSM(ThisEvent);
            RightWheelSpeed(0);
            LeftWheelSpeed(0);
            
            if (ThisEvent.EventType == SHOOTING_3PT_DONE){
                CurrentState = Reload;
                ES_Timer_InitTimer(RELOAD_TIMER, RELOAD_TICKS);
            }
            break;
            
        case Reload:
            
            if (ThisEvent.EventType == ES_TIMEOUT){
                if (ThisEvent.EventParam == RELOAD_TIMER){
                    CurrentState = Find_Beacon;
                }
            }
            
        default: // all unhandled states fall into here
            break;
    } // end switch on Current State
    
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/
