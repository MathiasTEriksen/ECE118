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
#include "TwoPointerSubHSM.h"
#include "Servo.h"
#include "Motor_Driver.h"
#include "BCEventChecker.h"
/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

#define TURN_2PT_TIMER 4
#define TURNL_2PT_TICKS 50
#define TURNR_2PT_TICKS 50

typedef enum {
    Init,
    Move_Fwd,
    Shooting,
    Turn,
    Turn_Back,        
} TwoPointerSubHSMState_t;

static const char *StateNames[] = {
	"Init",
	"Move_Fwd",
    "Shooting"
	"Turn",
	"Turn_Back",
};

//static unsigned char Side;
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

static TwoPointerSubHSMState_t CurrentState = Init; // <- change name to match ENUM
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
uint8_t InitTwoPointerSubHSM(void)
{
    CurrentState = Init;    
    //Side = CheckSide();

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
ES_Event RunTwoPointerSubHSM(ES_Event ThisEvent)
{
 switch (CurrentState) {
        case Init: // If current state is initial Psedudo State
            CurrentState = Turn;
            break;

        case Turn: // in the first state, replace this with correct names
            if (Side == LEFT){
                LeftWheelSpeed(300);
                RightWheelSpeed(0);
                if (Analog_TapeRead_L() < 450){
                    ES_Timer_InitTimer(TURN_2PT_TIMER, 10);   
                    CurrentState = Shooting;
                } else {
                    ES_Timer_InitTimer(TURN_2PT_TIMER, TURNL_2PT_TICKS);   
                    CurrentState = Shooting;
                }
            } 
            else if (Side == RIGHT){
                LeftWheelSpeed(0);
                RightWheelSpeed(300);
                if (Analog_TapeRead_R() < 450){
                    ES_Timer_InitTimer(TURN_2PT_TIMER, 10);
                    CurrentState = Shooting;
                } else {
                    ES_Timer_InitTimer(TURN_2PT_TIMER, TURNL_2PT_TICKS);   
                CurrentState = Shooting;
                }
            }  
            
            break;
            
        case Shooting:
            if (ThisEvent.EventType == ES_TIMEOUT){
                if (ThisEvent.EventParam == TURN_2PT_TIMER){
                    LeftWheelSpeed(0);
                    RightWheelSpeed(0);
                    ES_Timer_InitTimer(BALL_RELEASE_TIMER, 999);
                    ES_Timer_InitTimer(SHOOT_TIMER, SHOOT_TICKS);
                    Send_Ball();
                    
                } 
                if (ThisEvent.EventParam == BALL_RELEASE_TIMER){
                    //ES_Timer_InitTimer(TURN_1PT_TIMER, TURN_1PT_TICKS);
                    Stop_Ball();
                    //ES_Timer_InitTimer(TURN_1PT_TIMER, TURN_1PT_TICKS);
                }
                if (ThisEvent.EventParam == SHOOT_TIMER){
                    if (Side == RIGHT) {
                        ES_Timer_InitTimer(TURN_2PT_TIMER, TURNR_2PT_TICKS+40); 
                        CurrentState = Turn_Back;
                    }
                    else if (Side == LEFT){
                        ES_Timer_InitTimer(TURN_2PT_TIMER, TURNL_2PT_TICKS+40); 
                        CurrentState = Turn_Back;
                    }
                        
                }
                
            }
            break;
        case Turn_Back:
            if (Side == LEFT){
                LeftWheelSpeed(-300);
                RightWheelSpeed(0);
            } 
            else if (Side == RIGHT){
                LeftWheelSpeed(0);
                RightWheelSpeed(-300);
            }
            
            if (ThisEvent.EventType == ES_TIMEOUT){ 
                if (ThisEvent.EventParam == TURN_2PT_TIMER){
                   ThisEvent.EventType = SHOOTING_2PT_DONE;  
                    CurrentState = Init;
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

