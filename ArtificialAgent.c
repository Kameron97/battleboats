/* 
 * File:    ArtificialAgent.c
 * Author1: Hasson Qayum
 * Author2: Kameron Gill
 */

// **** Include libraries here ****
// Standard libraries
#include <stdio.h> 
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
//CMPE13 Support Library
#include "Agent.h"
#include "CircularBuffer.h"
#include "Oled.h"
#include "Buttons.h"
#include "Protocol.h"
#include "Uart1.h"
#include "Field.h"
#include "OledDriver.h"
#include "FieldOled.h"

// Microchip libraries
// #include <xc.h>
// #include <plib.h>

// User libraries

// **** Set any macros or preprocessor directives here ****

// **** Declare any data types here ****

// **** Define any module-level, global, or external variables here ****
static Field yourField, enemyField;
static GuessData yourGData, enemyGData; // variables for guess data
static NegotiationData yourNData, enemyNData; // variables for negotiation data
static AgentState state = AGENT_STATE_GENERATE_NEG_DATA; // track state machine
static FieldOledTurn status = FIELD_OLED_TURN_NONE; 
static ProtocolParserStatus proParserStatus; // return value of ProtocolDecode()
static TurnOrder getTurnOrder; // variable for TurnOrder from Protocol.c
static AgentEvent getAgentEvent; // values based on return value of ProtocolDecode()

static int stringLength;
static uint8_t validNData;
static uint8_t oppData;
static int i;

// **** Declare any function prototypes here ****


/**
 * The Init() function for an Agent sets up everything necessary for an agent before the game
 * starts. This can include things like initialization of the field, placement of the boats,
 * etc. The agent can assume that stdlib's rand() function has been seeded properly in order to
 * use it safely within.
 */
void AgentInit(void){
    FieldInit(&yourField, FIELD_POSITION_EMPTY);
    FieldInit(&enemyField, FIELD_POSITION_UNKNOWN);

    int value = 0;

    while (value != 1){
    	FieldAddBoat(&yourField, rand()%FIELD_ROWS, rand()%FIELD_COLS, rand()%4, FIELD_BOAT_SMALL);
    	FieldAddBoat(&yourField, rand()%FIELD_ROWS, rand()%FIELD_COLS, rand()%4, FIELD_BOAT_MEDIUM);
    	FieldAddBoat(&yourField, rand()%FIELD_ROWS, rand()%FIELD_COLS, rand()%4, FIELD_BOAT_LARGE);
    	FieldAddBoat(&yourField, rand()%FIELD_ROWS, rand()%FIELD_COLS, rand()%4, FIELD_BOAT_HUGE);

    	value = 1;
    }
}

/**
 * The Run() function for an Agent takes in a single character. It then waits until enough
 * data is read that it can decode it as a full sentence via the Protocol interface. This data
 * is processed with any output returned via 'outBuffer', which is guaranteed to be 255
 * characters in length to allow for any valid NMEA0183 messages. The return value should be
 * the number of characters stored into 'outBuffer': so a 0 is both a perfectly valid output and
 * means a successful run.
 * @param in The next character in the incoming message stream.
 * @param outBuffer A string that should be transmit to the other agent. NULL if there is no
 *                  data.
 * @return The length of the string pointed to by outBuffer (excludes \0 character).
 */
int AgentRun(char in, char *outBuffer){
    
//    if(in != NULL){
//        // decode enemy guess data
//        proParserStatus = ProtocolDecode(in, &enemyNData, &enemyGData);
//        // check each ParserStatus to get AgentEvent
//        if(proParserStatus == PROTOCOL_PARSING_FAILURE){
//            getAgentEvent = AGENT_EVENT_MESSAGE_PARSING_FAILED;
//        } else if (proParserStatus == PROTOCOL_WAITING){
//            getAgentEvent = AGENT_EVENT_NONE;
//        } else if(proParserStatus == PROTOCOL_PARSING_GOOD){
//            getAgentEvent = AGENT_EVENT_NONE;
//        } else if(proParserStatus == PROTOCOL_PARSED_COO_MESSAGE){
//            getAgentEvent = AGENT_EVENT_RECEIVED_COO_MESSAGE;
//        } else if(proParserStatus == PROTOCOL_PARSED_HIT_MESSAGE){
//            getAgentEvent = AGENT_EVENT_RECEIVED_HIT_MESSAGE;
//        } else if(proParserStatus == PROTOCOL_PARSED_CHA_MESSAGE){
//            getAgentEvent = AGENT_EVENT_RECEIVED_CHA_MESSAGE;
//        } else if(proParserStatus == PROTOCOL_PARSED_DET_MESSAGE){
//            getAgentEvent = AGENT_EVENT_RECEIVED_DET_MESSAGE;
//        } else {
//            printf("ERROR\n");
//        }
//    }
    
    switch(state){
    case AGENT_STATE_GENERATE_NEG_DATA:
        // arrow to SEND_DATA
        ProtocolGenerateNegotiationData(&yourNData);
        stringLength = ProtocolEncodeChaMessage(outBuffer, &yourNData);
        state = AGENT_STATE_SEND_CHALLENGE_DATA;
        return stringLength;
        break;
    case AGENT_STATE_SEND_CHALLENGE_DATA:
        proParserStatus = ProtocolDecode(in, &enemyNData, NULL);
        // arrow to DETERMINE_ORDER
        if(proParserStatus == PROTOCOL_PARSED_CHA_MESSAGE){
            oppData = ProtocolValidateNegotiationData(&enemyNData);
            state = AGENT_STATE_DETERMINE_TURN_ORDER;
            stringLength = ProtocolEncodeDetMessage(outBuffer, &yourNData);
            return stringLength;
        } else if(proParserStatus == PROTOCOL_PARSING_FAILURE) {
            // arrow to INVALID
            OledClear(OLED_COLOR_BLACK);
            OledDrawString(AGENT_ERROR_STRING_PARSING);
            OledUpdate();
            state = AGENT_STATE_INVALID;
            return STANDARD_ERROR;
        }
        break;
    case AGENT_STATE_DETERMINE_TURN_ORDER:
        proParserStatus = ProtocolDecode(in, &enemyNData, NULL);
        // arrow to option state
        if(proParserStatus == PROTOCOL_PARSED_DET_MESSAGE){
            validNData = ProtocolValidateNegotiationData(&enemyNData);
            if(validNData == SUCCESS){
                getTurnOrder = ProtocolGetTurnOrder(&yourNData, &enemyNData);
                // arrow to SEND_GUESS
                if(getTurnOrder == TURN_ORDER_START){
                    state = AGENT_STATE_SEND_GUESS;
                    status = FIELD_OLED_TURN_MINE;
                    FieldOledDrawScreen(&yourField, &enemyField, status);
                } else if(getTurnOrder == TURN_ORDER_DEFER) {
                    // arrow to WAIT_FOR_GUESS
                    state = AGENT_STATE_WAIT_FOR_GUESS;
                    status = FIELD_OLED_TURN_THEIRS;
                    FieldOledDrawScreen(&yourField, &enemyField, status);
                } else if(getTurnOrder == TURN_ORDER_TIE){
                    // arrow to INVALID for TIE
                    OledClear(OLED_COLOR_BLACK);
                    OledDrawString(AGENT_ERROR_STRING_ORDERING);
                    OledUpdate();
                    state = AGENT_STATE_INVALID;
                    return STANDARD_ERROR;
                }
            } else {
                // arrow to INVALID for Invalid Negotiation Data
                OledClear(OLED_COLOR_BLACK);
                OledDrawString(AGENT_ERROR_STRING_NEG_DATA);
                OledUpdate();
                state = AGENT_STATE_INVALID;
                return STANDARD_ERROR;
            }
        } else if(proParserStatus == PROTOCOL_PARSING_FAILURE) {
            // arrow to INVALID
            OledClear(OLED_COLOR_BLACK);
            OledDrawString(AGENT_ERROR_STRING_PARSING);
            OledUpdate();
            state = AGENT_STATE_INVALID;
            return STANDARD_ERROR;
        }
        break;
    case AGENT_STATE_SEND_GUESS:
    	// int i;
    	
        for (i = 0; i < (BOARD_GetPBClock() / 8); i++);

        while (FieldAt(&enemyField, enemyGData.row, enemyGData.col) != FIELD_POSITION_UNKNOWN){

            enemyGData.row =  (rand() % (FIELD_ROWS));
            enemyGData.col =  (rand() % (FIELD_ROWS));
        }
        state = AGENT_STATE_WAIT_FOR_HIT;
        return ProtocolEncodeCooMessage(outBuffer, &yourGData);
        break;

        
        
        
    case AGENT_STATE_WAIT_FOR_HIT:
    if (getAgentEvent == PROTOCOL_PARSED_HIT_MESSAGE){
    	if(AgentGetEnemyStatus != 0){
    		FieldUpdateKnowledge(&enemyField, &enemyGData );
    		FieldOledDrawScreen(&yourField, &enemyField, FIELD_OLED_TURN_THEIRS);
    		state = AGENT_STATE_WAIT_FOR_GUESS;
    	}
    	else{
    		FieldOledDrawScreen(&yourField, &enemyField, FIELD_OLED_TURN_NONE);
    		state = AGENT_STATE_WON;
    	}
    }
        break;
    
        
        
    case AGENT_STATE_WAIT_FOR_GUESS:
        if (getAgentEvent == PROTOCOL_PARSED_COO_MESSAGE){
        	if (AgentGetStatus() == 0){
        		FieldOledDrawScreen(&yourField, &enemyField, FIELD_OLED_TURN_NONE);
        		state = AGENT_STATE_LOST;
        	}
        else {
        	FieldRegisterEnemyAttack(&yourField, &enemyGData);
        	FieldOledDrawScreen(&yourField, &enemyField, FIELD_OLED_TURN_NONE);
        	state = AGENT_STATE_SEND_GUESS;
        }
        return ProtocolEncodeHitMessage(outBuffer, &enemyGData);
    }

        break;
        
        
    case AGENT_STATE_INVALID:
        return FALSE;
        break;
        
        
    case AGENT_STATE_LOST:
        return FALSE;
        break;
        
        
    case AGENT_STATE_WON:
        return FALSE;
        break;
        
        
    default:
        
        break;
    }
}

/**
 * StateCheck() returns a 4-bit number indicating the status of that agent's ships. The smallest
 * ship, the 3-length one, is indicated by the 0th bit, the medium-length ship (4 tiles) is the
 * 1st bit, etc. until the 3rd bit is the biggest (6-tile) ship. This function is used within
 * main() to update the LEDs displaying each agents' ship status. This function is similar to
 * Field::FieldGetBoatStates().
 * @return A bitfield indicating the sunk/unsunk status of each ship under this agent's control.
 *
 * @see Field.h:FieldGetBoatStates()
 * @see Field.h:BoatStatus
 */
uint8_t AgentGetStatus(void){
    return FieldGetBoatStates(&yourField);
}

/**
 * This function returns the same data as `AgentCheckState()`, but for the enemy agent.
 * @return A bitfield indicating the sunk/unsunk status of each ship under the enemy agent's
 *         control.
 *
 * @see Field.h:FieldGetBoatStates()
 * @see Field.h:BoatStatus
 */
uint8_t AgentGetEnemyStatus(void){
    return FieldGetBoatStates(&enemyField);
}
