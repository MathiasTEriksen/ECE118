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

unsigned char Analog_TapeInit(void);

uint16_t Analog_TapeRead_L(void);
uint16_t Analog_TapeRead_R(void);

uint16_t Analog_TapeRead_FL(void);
uint16_t Analog_TapeRead_FR(void);