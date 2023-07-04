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
#include "ES_Configure.h"
#include "BCEventChecker.h"
#include "BumperSensor.h"
#include "ES_Framework.h"
#include "ES_Events.h"

#include "BOARD.h"
#include "serial.h"
#include "AD.h"
#include "IO_Ports.h"
#include "pwm.h"

#include <xc.h>
#include <stdio.h>

//All Ports Below are for PORTX
#define BumperInFrontLeft PIN7
#define BumperInFrontRight PIN8
#define BumperInBackLeft PIN9
#define BumperInBackRight PIN10

#define BumperFrontLeft 0b1000
#define BumperFrontRight 0b0100
#define BumperBackLeft 0b0010
#define BumperBackRight 0b0001

unsigned char Bumper_Init(void) {
    IO_PortsSetPortInputs(PORTX, (BumperInBackLeft | BumperInBackRight | BumperInFrontLeft | BumperInFrontRight));
    return SUCCESS;
}

/**
 * @function BumperRead(void)
 * @param None
 * @return 1 or 0 for lower 4-bits, FLeft = 0b1000, FRight = 0b0100, BLeft = 0b0010, BRight = 0b0001 
 * @brief Reads Bumpers out as a 8-bit value where each of the lower four bits
 *        represents a bumper
 * @author Leo King */
uint8_t BumperRead(void) {
    uint16_t PORTXPINS = IO_PortsReadPort(PORTX);
    uint8_t Bumper = 0;
    
    if ((PORTXPINS & BumperInFrontLeft)) {
        Bumper |= BumperFrontLeft;
    }
    if ((PORTXPINS & BumperInFrontRight)) {
        Bumper |= BumperFrontRight;
    }
    if ((PORTXPINS & BumperInBackLeft)) {
        Bumper |= BumperBackLeft;
    }
    if ((PORTXPINS & BumperInBackRight)) {
        Bumper |= BumperBackRight;
    }
    return Bumper;
}

/* *****************************************************************************
 End of File
 */
