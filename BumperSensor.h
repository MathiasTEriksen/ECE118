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

#ifndef BumperSensor_H    /* Guard against multiple inclusion */
#define BumperSensor_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */
#include "ES_Configure.h"
#include "BCEventChecker.h"
#include "ES_Framework.h"
#include "ES_Events.h"

#include "BOARD.h"
#include "serial.h"
#include "AD.h"
#include "IO_Ports.h"
#include "pwm.h"

#include <xc.h>
#include <stdio.h>

//Initializes the Pins to be read
unsigned char Bumper_Init();

/**
 * @function BumperRead(void)
 * @param None
 * @return 1 or 0 for lower 4-bits, FLeft = 0b1000, FRight = 0b0100, BLeft = 0b0010, BRight = 0b0001 
 * @brief Reads Bumpers out as a 8-bit value where each of the lower four bits
 *        represents a bumper
 * @author Leo King */
uint8_t BumperRead(void);
#endif /* BumperSensor */

/* *****************************************************************************
 End of File
 */
