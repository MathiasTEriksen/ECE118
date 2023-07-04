/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef Motor_Driver_H    /* Guard against multiple inclusion */
#define Motor_Driver_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/**
 * @Function Motors_Init(void)
 * @param none
 * @return TRUE or FALSE for success
 * @brief This function is a initializer function for driving the motors. Adds
 * the pins needed to drive the motors using an H-Bridge.
 * @note Template made from Roach.c/h
 * @author Leo King 5/16/2023
 **/
char Motors_Init(void);

/**
 * @Function void LeftWheelSpeed(int PWM)
 * @param PWM, a value ranging from -1000 to 1000
 * @return TRUE or FALSE for success
 * @brief Drives the left wheel forward when a positive value is inputted and 
 * in reverse when a negative value is given.
 * @note Template made from Roach.c/h
 * @author Leo King 5/16/2023
 **/
int LeftWheelSpeed(int PWM);

/**
 * @Function void LeftWheelSpeed(int PWM)
 * @param PWM, a value ranging from -1000 to 1000
 * @return TRUE or FALSE for success
 * @brief Drives the right wheel forward when a positive value is inputted and 
 * in reverse when a negative value is given.
 * @note Template made from Roach.c/h
 * @author Leo King 5/16/2023
 **/
int RightWheelSpeed(int PWM);

#endif /* Motor_Driver_H */

/* *****************************************************************************
 End of File
 */
