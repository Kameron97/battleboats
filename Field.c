//Kameron Gill 
//Hasam Quyam

//Field.c

#include "Field.h"
#include "BOARD.h"
#include "Protocol.h"


//functions below================
void FieldInit(Field *f, FieldPosition p){
	int i = 0;//row
	int j = 0;//columns

    while (i < FIELD_ROWS) { 
        while ( j < FIELD_COLS) {
            f->field[i][j] = p;
            j++;
        }
        i++;
    }

    // init boat lives for each boat type
    f->hugeBoatLives = FIELD_BOAT_LIVES_HUGE; 
    f->largeBoatLives = FIELD_BOAT_LIVES_LARGE;
    f->smallBoatLives = FIELD_BOAT_LIVES_SMALL;
    f->mediumBoatLives = FIELD_BOAT_LIVES_MEDIUM;

}


FieldPosition FieldAt(const Field *f, uint8_t row, uint8_t col){
	    return f->field[row][col];

}


FieldPosition FieldSetLocation(Field *f, uint8_t row, uint8_t col, FieldPosition p){
    FieldPosition temp = f->field[row][col];
    f->field[row][col] = p;
    return temp;

}

//got help from my tutor on this function
uint8_t FieldAddBoat(Field *f, uint8_t row, uint8_t col, BoatDirection dir, BoatType type){
    int boatLength;
    BoatDirection tempDir;
    BoatType tempBoat;

    if( tempBoat == FIELD_BOAT_SMALL){
        boatLength = 2;
        tempBoat = FIELD_POSITION_SMALL_BOAT
    }

    else if (tempBoat == FIELD_BOAT_MEDIUM){
        boatLength = 3;
        tempBoat = FIELD_POSITION_MEDIUM_BOAT;
    }
    else if (tempBoat == FIELD_BOAT_LARGE){
        boatLength = 4;
        tempBoat = FIELD_POSITION_LARGE_BOAT;
    }
    else if (tempBoat == FILED_BOAT_HUGE){

        boatLength = 5;
        tempBoat = FIELD_POSITION_HUGE_BOAT;
    }
    else{
        boatLength = 0;
        return STANDARD_ERROR;
    }


    switch(dir){
        case FIELD_BOAT_DIRECTION_NORTH:
            int i = 0;
            for (i = 0; i < boatLength; i++) {
                if (row - i < 0) 
                    return FALSE;
            
            if (f->field[row - i][col] != FIELD_POSITION_EMPTY) 
                return FALSE;
            
            }
            i = 0;
            for (i = 0; i < boatLength; i++) {
                f->field[row - i][col] = tempBoat;
            }
        case FIELD_BOAT_DIRECTION_SOUTH:
         int i = 0;
        for (i = 0; i < length; i++) {
            if (row + i >= FIELD_ROWS) {
                return FALSE;
            }
            if (f->field[row + i][col] != FIELD_POSITION_EMPTY) {
                return FALSE;
            }
        }
        i = 0;
        for (i = 0; i < length; i++) {
            f->field[row + i][col] = tempBoat;
        }


        case FIELD_BOAT_DIRECTION_EAST:
        int i = 0;
        for (i = 0; i < length; i++) {
            if (col + i >= FIELD_COLS) {
                return FALSE;
            }
            if (f->field[row][col + i] != FIELD_POSITION_EMPTY) {
                return FALSE;
            }
        }
        i = 0;
        for (i = 0; i < length; i++) {
            f->field[row][col + i] = tempBoat;
        }



        case FIELD_BOAT_DIRECTION_WEST:
                int i = 0;
        for (i = 0; i < length; i++) {
            if (col - i < 0) {
                return FALSE;
            }
            if (f->field[row][col - i] != FIELD_POSITION_EMPTY) {
                return FALSE;
            }
        }
        i = 0;
        for (i = 0; i < length; i++) {
            f->field[row][col - i] = tempBoat;
        }
        //        return TRUE;
    


    }
    return TRUE;

    }





FieldPosition FieldRegisterEnemyAttack(Field *f, GuessData *gData){
    int temp = FieldAt(f, gData->row, gData->col);

    if (temp == FIELD_POSITION_SMALL_BOAT){
        f->smallBoatLives--;
        f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
        gData->hit = FIELD_POSITION_HIT;
    }
    else if (temp == FIELD_POSITION_MEDIUM_BOAT){
                    f->mediumBoatLives--;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            gData->hit = FIELD_POSITION_HIT;
    }
    else if ( temp == FIELD_POSITION_LARGE_BOAT){
         f->largeBoatLives--;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            gData->hit = FIELD_POSITION_HIT;
    }
    else if (temp == FIELD_POSITION_HUGE_BOAT){
        f->hugeBoatLives--;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            gData->hit = FIELD_POSITION_HIT;
        }else {
            f->field[gData->row][gData->col] = FIELD_POSITION_MISS;
            gData->hit = FIELD_POSITION_MISS;
        }

        return temp;

}

FieldPosition FieldUpdateKnowledge(Field *f, const GuessData *gData){
    FieldPosition temp = f->field[gData->row][gData->col];


    if (gData->hit == HIT_MISS)
        f->field[gData->row][gData->col] = FIELD_POSITION_EMPTY;
    else if (gData->hit != HIT_MISS){


        switch (gData->hit){

            case HIT_HIT:
                f->field[gData->row][gData->col];
                break;
            case HIT_SUNK_MEDIUM_BOAT:
                f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
                f->mediumBoatLives = 0;
                break;
            case HIT_SUNK_SMALL_BOAT:
                f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
                f->smallBoatLives = 0;
                break;
            case HIT_SUNK_LARGE_BOAT:
                f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
                f->largeBoatLives = 0;
                break;
            case HIT_SUNK_HUGE_BOAT:
                f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
                f->hugeBoatLives = 0;
                break;
        }
    }
    return temp;
}

uint8_t FieldGetBoatStates(const Field *f){

}


