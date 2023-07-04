/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "Motor_Driver.h"

#include "BOARD.h"
#include "serial.h"
#include "AD.h"
#include "IO_Ports.h"
#include "pwm.h"

#include <xc.h>
#include <stdio.h>
#include <stdint.h>

//#define MotorTests
#define ALL_16Bits 0xFFFF
#define MAX_FORWARD 1000
#define MAX_REVERSE -1000

#define Left_Wheel_PWM PWM_PORTY10
#define Right_Wheel_PWM PWM_PORTY12

#define Left_FlyWheel_PWM PWM_PORTY04
#define Right_FlyWheel_PWM PWM_PORTZ06


//Below References PORTX only
#define IN1 PIN11 
#define IN2 PIN12
#define IN3 PIN5
#define IN4 PIN6

//Below References PORTZ
#define IN5 PIN5
#define IN6 PIN9
#define IN7 PIN7
#define IN8 PIN8



 

/**
 * @Function Motors_Init(void)
 * @param none
 * @return TRUE or FALSE, To Denote Success or Failure
 * @brief This function is a initializer function for driving the motors.
 * @note Template made from Roach.c/h
 * @author Leo King 5/16/2023
 **/
char Motors_Init(void) {
    char returnVal = PWM_Init();
    if (returnVal == ERROR) {
        return ERROR;
    }
    PWM_AddPins(Left_Wheel_PWM);                //ENA For H-Bridge Left Wheel
    PWM_AddPins(Right_Wheel_PWM);               //ENB For H-Bridge Right Wheel
    PWM_AddPins(Left_FlyWheel_PWM);                //ENA For H-Bridge Left Wheel
    PWM_AddPins(Right_FlyWheel_PWM);               //ENB For H-Bridge Right Wheel
    
    PWM_SetFrequency(MIN_PWM_FREQ);
    PWM_SetDutyCycle(Left_Wheel_PWM, MIN_PWM);  //Turn Motor Speed to Zero
    PWM_SetDutyCycle(Right_Wheel_PWM, MIN_PWM); //Turn Motor Speed to Zero
    
    PWM_SetDutyCycle(Left_FlyWheel_PWM, MIN_PWM);  //Turn Motor Speed to Zero
    PWM_SetDutyCycle(Right_FlyWheel_PWM, MIN_PWM); //Turn Motor Speed to Zero
    
    IO_PortsSetPortOutputs(PORTX, IN1  | IN2 | IN3 | IN4);      //Set ALL PORTS TO OUTPUT
    IO_PortsClearPortBits(PORTX,  IN1  | IN2 | IN3 | IN4);       //SETS ALL PORTS LOW
    
    IO_PortsSetPortOutputs(PORTZ, IN5  | IN6 | IN7 | IN8);      //Set ALL PORTS TO OUTPUT
    IO_PortsClearPortBits(PORTZ,  IN5  | IN6 | IN7 | IN8);       //SETS ALL PORTS LOW
    
    //Initialize Both Wheels Directions to Forward
    IO_PortsWritePort(PORTX, IN1 | IN3);
    IO_PortsWritePort(PORTZ, IN5 | IN7);
    
    return SUCCESS;
}

/**
 * @Function void LeftWheelSpeed(int PWM)
 * @param PWM, a value ranging from -1000 to 1000
 * @return TRUE or FALSE for success
 * @brief Drives the left wheel forward when a positive value is inputted and 
 * in reverse when a negative value is given.
 * @note Template made from Roach.c/h
 * @author Leo King 5/16/2023
 **/
int LeftWheelSpeed(int PWM) {
    if ((PWM > MAX_FORWARD) || (PWM < MAX_REVERSE)) {
        return FALSE;
    }
    //Forward
    if (PWM >= 0) {                     
        IO_PortsSetPortBits(PORTX, IN1);
        IO_PortsClearPortBits(PORTX, IN2);
        PWM_SetDutyCycle(Left_Wheel_PWM, PWM);
    }
    //Reverse
    else {
        IO_PortsSetPortBits(PORTX, IN2);
        IO_PortsClearPortBits(PORTX, IN1);
        PWM_SetDutyCycle(Left_Wheel_PWM, -PWM);
    }
    return TRUE;
}

/**
 * @Function void LeftWheelSpeed(int PWM)
 * @param PWM, a value ranging from -1000 to 1000
 * @return TRUE or FALSE for success
 * @brief Drives the right wheel forward when a positive value is inputted and 
 * in reverse when a negative value is given.
 * @note Template made from Roach.c/h
 * @author Leo King 5/16/2023
 **/
int RightWheelSpeed(int PWM) {
        if ((PWM > MAX_FORWARD) || (PWM < MAX_REVERSE)) {
        return FALSE;
    }
    //Forward
    if (PWM >= 0) {                     
        IO_PortsSetPortBits(PORTX, IN3);
        IO_PortsClearPortBits(PORTX, IN4);
        PWM_SetDutyCycle(Right_Wheel_PWM, PWM);
    }
    //Reverse
    else {
        IO_PortsSetPortBits(PORTX, IN4);
        IO_PortsClearPortBits(PORTX, IN3);
        PWM_SetDutyCycle(Right_Wheel_PWM, -PWM);
    }
    return TRUE;
}

/**
 * @Function void LeftFlyWheelSpeed(int PWM)
 * @param PWM, a value ranging from -1000 to 1000
 * @return TRUE or FALSE for success
 * @brief Drives the left flywheel forward when a positive value is inputted and 
 * in reverse when a negative value is given.
 * @note Template made from Roach.c/h
 * @author Leo King 5/16/2023
 **/
int LeftFlyWheelSpeed(int PWM) {
    if ((PWM > MAX_FORWARD) || (PWM < MAX_REVERSE)) {
        return FALSE;
    }
    //Forward
    if (PWM >= 0) {                     
        IO_PortsSetPortBits(PORTZ, IN5);
        IO_PortsClearPortBits(PORTZ, IN6);
        PWM_SetDutyCycle(Left_FlyWheel_PWM, PWM);
    }
    //Reverse
    else {
        IO_PortsSetPortBits(PORTZ, IN6);
        IO_PortsClearPortBits(PORTZ, IN5);
        PWM_SetDutyCycle(Left_FlyWheel_PWM, -PWM);
    }
    return TRUE;
}

int RightFlyWheelSpeed(int PWM) {
    if ((PWM > MAX_FORWARD) || (PWM < MAX_REVERSE)) {
        return FALSE;
    }
    //Forward
    if (PWM >= 0) {                     
        IO_PortsSetPortBits(PORTZ, IN7);
        IO_PortsClearPortBits(PORTZ, IN8);
        PWM_SetDutyCycle(Right_FlyWheel_PWM, PWM);
    }
    //Reverse
    else {
        IO_PortsSetPortBits(PORTZ, IN8);
        IO_PortsClearPortBits(PORTZ, IN7);
        PWM_SetDutyCycle(Right_FlyWheel_PWM, -PWM);
    }
    return TRUE;
}

#ifdef MotorTests
int main(void) {
    BOARD_Init();
    SERIAL_Init();
    
    char ret = Motors_Init();
    //printf("Returned: %d\r\n", ret);
    //RightWheelSpeed(-200);
    //LeftWheelSpeed(200);
    RightFlyWheelSpeed(-500);
    LeftFlyWheelSpeed(500);
    while(1) {
        ;//printf("Read PORTX: %u\r\n", IO_PortsReadPort(PORTX));
    }
    BOARD_End();
}
#endif

/* *****************************************************************************
 End of File
 */
