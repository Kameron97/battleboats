//Kameron Gill
//Hasam

//Lab 09- Battle Boats

//ArtificialAgent.c

#include "BOARD.h"
#include <xc.h>
#include <plib.h>

#include "Agent.h"
#include "CircularBuffer.h"
#include "Leds.h"
#include "Oled.h"
#include "Buttons.h"
#include "Protocol.h"
#include "Uart1.h"
#include "Field.h"
#include "OledDriver.h"
#include "FieldOled.h"


#include <stdio.h>
#include <string.h>


//variables ======================================================================
Field yourField, enemyField;







void AgentInit(void){

	FieldInit(&yourField, FIELD_POSITION_EMPTY);
    FieldInit(&enemyField, FIELD_POSITION_UNKNOWN);

}

int AgentRun(char in, char *outBuffer){

}

uint8_t AgentGetStatus(void){
	return FieldGetBoatStates(&yourField);

}


uint8_t AgentGetEnemyStatus(void){
	return FieldGetBoatStates(&enemyField);

}