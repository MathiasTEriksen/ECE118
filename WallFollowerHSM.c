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

#include <string.h>
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
#include "OPBSubHSM.h"
#include "timers.h"

#include <xc.h>
#include <stdio.h>


/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

#define BATTERY_DISCONNECT_THRESHOLD 175

#define TAPE_SERVICE_TIMER 0
#define TIMER_0_TICKS 3

#define RELOAD_TIMER 6
#define RELOAD_TICKS 4000

#define TAPE_TIMER 7
#define TAPE_TICKS 2000

#define WALL_ALIGN_TIMER 8

#define FR 4
#define FL 8
#define BR 1
#define BL 2
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
    Align_F,
    Align_R,     
    BAlign,
    Pivot,
    Reverse_Pivot,
    Wait_For_Tape,
    Follow_Wall,
    Reverse_Wall,
    Test_Stop,
    Shoot_1PT,        
    Shoot_2PT,
    Reverse_To_2PT,        
    Shoot_3PT,
    Go_to_Reload,        
    Reload
} BdayFSMState_t;

static const char *StateNames[] = {
	"Init",
	"Find_Beacon",
	"Find_Wall",
	"Align_Wall",
	"Pivot",
	"Wait_For_Tape",
	"Follow_Wall",
	"Test_Stop",
	"Shoot_1PT",
	"Go_To_2PT",
	"Shoot_2PT",
	"Go_To_3PT",
	"Shoot_3PT",
};


static BdayFSMState_t CurrentState = Init; // <- change enum name to match ENUM
static uint8_t MyPriority;

static unsigned char Side;
static unsigned char Direction;
static unsigned char OnePoint;
static unsigned char TwoPoint;
static unsigned char ThreePoint;



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
    InitOnePointerSubHSM();
    //InitOPBSubHSM();
    InitTwoPointerSubHSM();
    InitThreePointerSubHSM();
    TurnStile_Init();
    //RightFlyWheelSpeed(-800);
    //LeftFlyWheelSpeed(-800);
    
    ES_Timer_InitTimer(TAPE_SERVICE_TIMER, TIMER_0_TICKS);
    
    Side = CheckSide();
    Stop_Ball();
    printf("WallFollowerHSM\r\n");
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
ES_Event RunBdayFSM(ES_Event ThisEvent)
{
    static unsigned char FRSensor = FALSE;
    static unsigned char FLSensor = FALSE;
    //static unsigned char Shoot3Flag = FALSE;
    static unsigned char TapeFlag = FALSE;
    static unsigned char Collision_Flag = FALSE;
    static unsigned char LastState;
    static uint32_t LastTime;
    int i;
    static uint32_t CurrentTime;
    
//    if (ThisEvent.EventType == RIGHT_WALL_INRANGE){
//        printf("RIGHT WALL INRANGE\r\n");
//    }
//    
//    if (ThisEvent.EventType == RIGHT_WALL_FAR){
//        printf("RIGHT WALL FAR\r\n");
//    }
    if (ThisEvent.EventType == FRONT_RIGHT_WALL_INRANGE){
        printf("FRONT RIGHT WALL INRANGE\r\n");
    }
    
    if (ThisEvent.EventType == FRONT_RIGHT_WALL_FAR){
        printf("FRONT RIGHT WALL FAR\r\n");
    }
    
    if (ThisEvent.EventType == FRONT_LEFT_WALL_INRANGE){
        printf("FRONT LEFT WALL INRANGE\r\n");
    }
    
    if (ThisEvent.EventType == FRONT_LEFT_WALL_FAR){
        printf("FRONT LEFT WALL FAR\r\n");
    }
//    if (ThisEvent.EventType == BUMPER_BUMPED) {
//        if (ThisEvent.EventParam == FL) printf("Front Left\r\n");
//        if (ThisEvent.EventParam == FR) printf("Front Right\r\n");
//        if (ThisEvent.EventParam == BL) printf("Back Left\r\n");
//        if (ThisEvent.EventParam == BR) printf("Back Right\r\n");
//        
//    }
//    if (ThisEvent.EventType == ON_WIRE) {
//        printf("ON_WIRE\r\n");        
//    }
//    if (ThisEvent.EventType == OFF_WIRE) {
//        printf("OFF_WIRE\r\n");        
//    }
//    if (ThisEvent.EventType == BEACON_PRESENT) {
//        printf("BEACON HERE\r\n");        
//    }
//    if (ThisEvent.EventType == BEACON_ABSENT) {
//        printf("BEACON HERE\r\n");        
//    }
       
    
    
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

            if (ThisEvent.EventParam == TAPE_TIMER){
                TapeFlag = TRUE;
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
            
            if (Side == RIGHT){
                LeftWheelSpeed(1000);
                RightWheelSpeed(-1000);
            } 
            else {
                LeftWheelSpeed(-1000);
                RightWheelSpeed(1000);
            }

            if (ThisEvent.EventType == BEACON_PRESENT){
                CurrentState = Find_Wall;
                ES_Timer_InitTimer(TAPE_TIMER, TAPE_TICKS);
                LeftWheelSpeed(500);
                RightWheelSpeed(500);
                LeftFlyWheelSpeed(-300);
                RightFlyWheelSpeed(-300);
                
                if (Side == RIGHT){
                    if (Analog_TapeRead_FR() < 250){
                        CurrentState = Pivot;
                        RightWheelSpeed(300);
                        LeftWheelSpeed(0);
                        
                        if (Analog_TapeRead_L() < 250){
                            CurrentState = Follow_Wall;
                            RightWheelSpeed(500);
                            LeftWheelSpeed(300);
                        }
                    }
                } 
                else if (Side == LEFT) {
                    if (Analog_TapeRead_FL() < 250){
                        CurrentState = Pivot;
                        RightWheelSpeed(0);
                        LeftWheelSpeed(300);
                        
                        if (Analog_TapeRead_L() < 250){
                            CurrentState = Follow_Wall;
                            RightWheelSpeed(300);
                            LeftWheelSpeed(500);
                        }
                    }
                }
            }
            break;
        
        case Find_Wall:
            
            if (Side == RIGHT){     

                LeftWheelSpeed(500);
                RightWheelSpeed(500);
                
                if (ThisEvent.EventType == FRONT_RIGHT_WALL_INRANGE){
                    CurrentState = Pivot;
                    RightWheelSpeed(300);
                    LeftWheelSpeed(0);
                    
                    if (Analog_TapeRead_R() < 250){
                        CurrentState = Follow_Wall;
                        RightWheelSpeed(500);
                        LeftWheelSpeed(300);
                    }
                }
            } 
            else if (Side == LEFT){
                
                LeftWheelSpeed(500);
                RightWheelSpeed(500);
                
                if (ThisEvent.EventType == FRONT_LEFT_WALL_INRANGE){
                    CurrentState = Pivot;
                    RightWheelSpeed(0);
                    LeftWheelSpeed(300);
                    
                    if (Analog_TapeRead_L() < 250){
                        CurrentState = Follow_Wall;
                        RightWheelSpeed(300);
                        LeftWheelSpeed(500);
                    }
                }
            }
            
            break;


        case Pivot: 
            
            if (Side == RIGHT){
                if (ThisEvent.EventType == BACK_RIGHT_WALL_INRANGE){
                    if (FRSensor){
                        CurrentState = Align_F;
                        LastState = Follow_Wall;
                        RightWheelSpeed(300);
                        LeftWheelSpeed(500);
                    } else {
                        CurrentState = Follow_Wall;
                        RightWheelSpeed(500);
                        LeftWheelSpeed(300);
                    }   
                }
                if (ThisEvent.EventType == FRONT_RIGHT_WALL_FAR){
                    FRSensor = TRUE;                    
                }
                
            } 
            else if (Side == LEFT){
                if (ThisEvent.EventType == BACK_LEFT_WALL_INRANGE){
                    if (FLSensor){
                        CurrentState = Align_F;
                        LastState = Follow_Wall;
                        RightWheelSpeed(500);
                        LeftWheelSpeed(300);
                    } 
                    else {
                        CurrentState = Follow_Wall;
                        RightWheelSpeed(300);
                        LeftWheelSpeed(500);
                    }
                }
                if (ThisEvent.EventType == FRONT_LEFT_WALL_FAR){
                    FLSensor = TRUE;
                }
            } 
            break;
         
        case Follow_Wall:     

            if ((ThisEvent.EventType == FRONT_LEFT_WALL_FAR) && (Side == LEFT)){
                LastState = CurrentState;
                CurrentState = Align_F;
                RightWheelSpeed(500);
                LeftWheelSpeed(300);
            }
            else if ((ThisEvent.EventType == FRONT_RIGHT_WALL_FAR) && (Side == RIGHT)){
                LastState = CurrentState;
                CurrentState = Align_F;
                RightWheelSpeed(300);
                LeftWheelSpeed(500);
            }  

            if ((ThisEvent.EventType == BACK_TAPE_TRIPPED) && (TapeFlag == TRUE)){
                    LeftWheelSpeed(0);
                    RightWheelSpeed(0);
                    ES_Timer_InitTimer(MOVE_FWD_TIMER, MOVE_FWD_TICKS);
                    CurrentState = Shoot_1PT;
                } 
            
            break;

        case Align_F:

            if (ThisEvent.EventType == FRONT_LEFT_WALL_INRANGE && Side == LEFT){
                RightWheelSpeed(300);
                LeftWheelSpeed(500);
                CurrentState = LastState;
            }
            if (ThisEvent.EventType == FRONT_RIGHT_WALL_INRANGE && Side == RIGHT){
                RightWheelSpeed(500);
                LeftWheelSpeed(300);
                CurrentState = LastState;
            }
            if ((ThisEvent.EventType == BACK_TAPE_TRIPPED) && (TapeFlag == TRUE)){
                    LeftWheelSpeed(0);
                    RightWheelSpeed(0);
                    ES_Timer_InitTimer(MOVE_FWD_TIMER, MOVE_FWD_TICKS);
                    CurrentState = Shoot_1PT;
            } 
            
            break;    


        case Shoot_1PT:   
            
            ThisEvent = RunOnePointerSubHSM(ThisEvent);
            
            if (ThisEvent.EventType == SHOOTING_1PT_DONE){                
                RightWheelSpeed(-500);
                LeftWheelSpeed(-500);
                CurrentState = Reverse_To_2PT;
            }
            break;
        
        case Reverse_To_2PT:
            
            if (ThisEvent.EventType == FRONT_TAPE_UNTRIPPED){
                RightWheelSpeed(0);
                LeftWheelSpeed(0);
                CurrentState = Shoot_2PT;
                ES_Timer_InitTimer(MOVE_FWD_TIMER, MOVE_FWD_TICKS);
            }
            
            break;
            
        case Reverse_Wall:
            
            if ((ThisEvent.EventType == BACK_LEFT_WALL_FAR) && Side == LEFT){
                LastState = CurrentState;
                CurrentState = Align_R;
                RightWheelSpeed(-500);
                LeftWheelSpeed(-300);
            }
            else if ((ThisEvent.EventType == BACK_RIGHT_WALL_FAR) && Side == RIGHT){
                LastState = CurrentState;
                CurrentState = Align_R;
                RightWheelSpeed(-300);
                LeftWheelSpeed(-500);
            }  

            if (ThisEvent.EventType == ON_WIRE){
                    LeftWheelSpeed(-500);
                    RightWheelSpeed(-500);
                    ES_Timer_InitTimer(MOVE_FWD_TIMER, MOVE_FWD_TICKS);
                    CurrentState = Shoot_3PT;
            }
            
            
            break;
            
        case Align_R:

             if (ThisEvent.EventType == BACK_LEFT_WALL_INRANGE && Side == LEFT){
                RightWheelSpeed(-500);
                LeftWheelSpeed(-300);
                CurrentState = LastState;
            }
            if (ThisEvent.EventType == BACK_RIGHT_WALL_INRANGE && Side == RIGHT){
                RightWheelSpeed(-300);
                LeftWheelSpeed(-500);
                CurrentState = LastState;
            } 

            if (ThisEvent.EventType == ON_WIRE){
                    LeftWheelSpeed(0);
                    RightWheelSpeed(0);
                    ES_Timer_InitTimer(MOVE_FWD_TIMER, MOVE_FWD_TICKS);
                    CurrentState = Shoot_3PT;
            }
            break; 


        case Shoot_2PT:

            ThisEvent = RunTwoPointerSubHSM(ThisEvent);
            
            if (ThisEvent.EventType == SHOOTING_2PT_DONE){
                
                if (Side == RIGHT){
                    if (Analog_TapeRead_R() < 250){
                        RightWheelSpeed(-500);
                        LeftWheelSpeed(-300); 
                        CurrentState = Reverse_Wall;
                    } else {
                       RightWheelSpeed(-300);
                        LeftWheelSpeed(-500); 
                        CurrentState = Align_R; 
                    }
                } else if (Side == LEFT){
                    if (Analog_TapeRead_L() < 250){
                        RightWheelSpeed(-300);
                        LeftWheelSpeed(-500); 
                        CurrentState = Reverse_Wall;
                    } else {
                       RightWheelSpeed(-500);
                        LeftWheelSpeed(-300); 
                        CurrentState = Align_R; 
                    }
                }   
                              
            }
            break;
             
        case Shoot_3PT:

            ThisEvent = RunThreePointerSubHSM(ThisEvent);

            if (ThisEvent.EventType == SHOOTING_3PT_DONE){
                CurrentState = Go_to_Reload;
                LeftWheelSpeed(-500);
                RightWheelSpeed(-500);
            }
            
            break;
            
        case Go_to_Reload:
            
            if (ThisEvent.EventType == FRONT_TAPE_UNTRIPPED) {
                ES_Timer_InitTimer(RELOAD_TIMER, RELOAD_TICKS);
                CurrentState = Reload;
            }                
            break;

        case Reload:

            RightWheelSpeed(0);
            LeftWheelSpeed(0);

            if (ThisEvent.EventType == ES_TIMEOUT){
                if (ThisEvent.EventParam == RELOAD_TIMER){
                    CurrentState = Follow_Wall;
                    ES_Timer_InitTimer(TAPE_TIMER, TAPE_TICKS);

                    FLSensor = FALSE;
                    FRSensor = FALSE;
//                    Shoot3Flag = FALSE;
                    TapeFlag = FALSE;

                    if (Side == RIGHT){
                        if (Analog_TapeRead_FR() < 250){
                            RightWheelSpeed(500);
                            LeftWheelSpeed(300);
                        } else {
                            RightWheelSpeed(300);
                            LeftWheelSpeed(500);
                        }
                    } else if (Side == LEFT){
                        if (Analog_TapeRead_FL() < 250){
                            RightWheelSpeed(300);
                            LeftWheelSpeed(500);
                        } else {
                            RightWheelSpeed(500);
                            LeftWheelSpeed(300);
                        }
                    }
                }
            }
            break;
            
        case Test_Stop:
           LeftFlyWheelSpeed(0);
           RightFlyWheelSpeed(0);
           RightWheelSpeed(0);
           LeftWheelSpeed(0);
           break;
            
        default: // all unhandled states fall into here
            break;
    } // end switch on Current State
    
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

