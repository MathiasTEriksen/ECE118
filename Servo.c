
#include "Servo.h"
#include "RC_Servo.h"
#include <xc.h>
#include <stdio.h>

//#define SERVO_TEST
#define TURNSTILE_PIN RC_PORTY07

#define MIDDLE 2000
#define OPEN 1000

unsigned char TurnStile_Init(void){
    
    RC_Init();
    RC_AddPins(TURNSTILE_PIN);
    return RC_SetPulseTime(TURNSTILE_PIN, MIDDLE);
    
}

void Stop_Ball(void){
    
    RC_SetPulseTime(TURNSTILE_PIN, MIDDLE);
    
}

void Send_Ball(void){
    
    RC_SetPulseTime(TURNSTILE_PIN, OPEN);
    
}

#ifdef SERVO_TEST


int main(){
    
    
    BOARD_Init();
    TurnStile_Init();
    
    int i;
    
    while(1){
        for(i=0; i<2000000; i++){
            asm("nop");
        }
        Send_Ball();
        for(i=0; i<2000000; i++){
            asm("nop");
        }
        Stop_Ball();
    }   
}
#endif

