#include "ES_Configure.h"
#include <stdio.h>
#include <xc.h>
#include "DigitalTapeSensors.h"

uint8_t CheckDigitalTape(void);
unsigned char CheckAnalogTape(void);
uint8_t CheckBumpers(void);
unsigned char CheckTrackWire(void);
unsigned char CheckBeacon(void);
uint8_t TemplateCheckBattery(void);
unsigned char CheckSide(void);