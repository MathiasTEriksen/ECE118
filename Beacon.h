
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
unsigned char Beacon_Init(void);

/**
 * @function BumperRead(void)
 * @param None
 * @return 1 for on track wire 0 for off
 * @brief Reads Track Wire Pin
 * @author Leo King */
uint8_t ReadBeacon(void);

/* *****************************************************************************
 End of File
 */
            
