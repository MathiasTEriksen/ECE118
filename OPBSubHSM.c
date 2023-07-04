/*
 * File: TemplateSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is for a substate machine. Make sure it has a unique name
 *
 * This is provided as an example and a good place to start.
 *
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
#include "BOARD.h"
#include "BdayFSM.h"
#include "OnePointerSubHSM.h"
#include "Servo.h"
#include "Motor_Driver.h"
#include "BCEventChecker.h"
/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

#define TURN_1PT_TIMER 2
#define TURN_1PT_TICKS 200

#define TURN_CONSTANT 100

typedef enum {
    Init,
    End,
    Timeout,        
    Find_Beacon,
    Turn_To_Shoot,
    Turn_Back,        
} OnePointerSubHSMState_t;

static const char *StateNames[] = {
	"Init",
	"Move_Fwd",
	"Turn",
	"Turn_Back",
};

//static unsigned char Side;
static first_run = 0;
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static OnePointerSubHSMState_t CurrentState = Init; // <- change name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitOPBSubHSM(void)
{
    CurrentState = Init;    
    Side = CheckSide();

    return TRUE;
}

/**
 * @Function RunTemplateSubHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 */
ES_Event RunOPBSubHSM(ES_Event ThisEvent)
{
    static uint32_t LastTime;
    static uint32_t CurrentTime;
    static uint32_t NewTime;

    switch (CurrentState) {
        case Init: // If current state is initial Psedudo State
            CurrentState = Timeout;
            
            break;

        case Timeout: // in the first state, replace this with correct names
            if (ThisEvent.EventType == ES_TIMEOUT){
                if (ThisEvent.EventParam == MOVE_FWD_TIMER){
                    LastTime = ES_Timer_GetTime();
                    if (Side == RIGHT){
                        LeftWheelSpeed(-300);
                        RightWheelSpeed(300);
                    } 
                    else if (Side == LEFT){
                        LeftWheelSpeed(300);
                        RightWheelSpeed(-300);
                    } 
                    CurrentState = Find_Beacon;
                }
            }
            break;
        case Find_Beacon:
            if (ThisEvent.EventType == BEACON_PRESENT){
                    CurrentState = Turn_To_Shoot;
                    CurrentTime = ES_Timer_GetTime();
                    NewTime = (CurrentTime - LastTime);
                    //NewTime = (NewTime/2) + ((TURN_CONSTANT*NewTime)/NewTime);
                    ES_Timer_InitTimer(TURN_1PT_TIMER, NewTime);

                    if (Side == RIGHT){
                        LeftWheelSpeed(0);
                        RightWheelSpeed(0);
                        LeftFlyWheelSpeed(-300);
                        RightFlyWheelSpeed(-300);
                    } 
                    else if (Side == LEFT){
                        LeftWheelSpeed(0);
                        RightWheelSpeed(0);
                        LeftFlyWheelSpeed(-300);
                        RightFlyWheelSpeed(-300);
                    }

            }
            break;
            
        case Turn_To_Shoot:
            if (ThisEvent.EventType == ES_TIMEOUT){
                if (ThisEvent.EventParam == TURN_1PT_TIMER){
                    LeftWheelSpeed(0);
                    RightWheelSpeed(0);
                    if (!first_run){
                        ES_Timer_InitTimer(BALL_RELEASE_TIMER, BALL_RELEASE_TICKS - 100);
                        ES_Timer_InitTimer(SHOOT_TIMER, SHOOT_TICKS);
                        Send_Ball();
                    }
                    else {
                        ES_Timer_InitTimer(BALL_RELEASE_TIMER, BALL_RELEASE_TICKS);
                        ES_Timer_InitTimer(SHOOT_TIMER, SHOOT_TICKS);
                        Send_Ball();
                    }
                } 
                if (ThisEvent.EventParam == BALL_RELEASE_TIMER){
                    Stop_Ball();
                }
                if (ThisEvent.EventParam == SHOOT_TIMER){
                    if (Side == LEFT) {
                        ES_Timer_InitTimer(TURN_1PT_TIMER, NewTime); 
                        CurrentState = Turn_Back;
                        
                    }
                    else {
                        ES_Timer_InitTimer(TURN_1PT_TIMER, NewTime);
                        CurrentState = Turn_Back;
                            if (Side == RIGHT){
                            LeftWheelSpeed(300);
                            RightWheelSpeed(-300);
                            } 
                            else if (Side == LEFT){
                            LeftWheelSpeed(300);
                            RightWheelSpeed(-300);
                        }
                    }
                        
                }
                
            }
            break;
        case Turn_Back:
//            if (Side == RIGHT){
//                        LeftWheelSpeed(-400);
//                        RightWheelSpeed(400);
//                    } 
//                    else if (Side == LEFT){
//                        LeftWheelSpeed(400);
//                        RightWheelSpeed(-400);
//                    }
//            
            if (ThisEvent.EventType == ES_TIMEOUT){ 
                if (ThisEvent.EventParam == TURN_1PT_TIMER){
                   ThisEvent.EventType = SHOOTING_1PT_DONE;  
                   CurrentState = Init;
                    //ES_Timer_InitTimer(TURN_1PT_TIMER, TURN_1PT_TICKS);
                }
            }
            break;
            
        default: // all unhandled events pass the event back up to the next level
            break;
    }
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

