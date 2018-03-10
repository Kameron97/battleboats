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

}


uint8_t FieldAddBoat(Field *f, uint8_t row, uint8_t col, BoatDirection dir, BoatType type){

}


FieldPosition FieldRegisterEnemyAttack(Field *f, GuessData *gData){

}

FieldPosition FieldUpdateKnowledge(Field *f, const GuessData *gData){

}

uint8_t FieldGetBoatStates(const Field *f){

}


