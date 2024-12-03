/*****************************************************************************
* lab2B.c for Lab2B of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/

#include "qpn_port.h"
#include "bsp.h"
#include "gameOn.h"
#include "lcd.h"
#include <math.h>

typedef struct GameOnTag  {               //Lab2B State machine
	QActive super;
} GemeOn;

int currentLevel = 0;

static QState GameOn_initial (GemeOn *me);
static QState GameOn_start (GemeOn *me);
static QState GameRunning (GemeOn *me);
static QState GameUpdate (GemeOn *me);
static QState GameOver (GemeOn *me);
// static QState GameOn_on      (gemeOn *me);
// static QState Lab2B_A  (gemeOn *me);
// static QState Lab2B_B  (gemeOn *me);

GemeOn newGame;

void GemeOn_ctor(void)  {
	GemeOn *me = &newGame;
	QActive_ctor(&me->super, (QStateHandler)&GameOn_initial);
}

QState GameOn_initial(GemeOn *me) {
    xil_printf("\n\rInitialization");
    return Q_TRAN(&GameOn_start);
}

QState GameOn_start(GemeOn *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            xil_printf("rendering init screen\n");
            // set init level
            changeLevel(currentLevel);
            // plot init frame(choose level and start game)
            initScreenPlot();
            return Q_HANDLED();
        }
        case ChangeLevelUp: {
            xil_printf("change mode up\n");
            currentLevel = (currentLevel == 2) ? 0 : currentLevel + 1;
            changeLevel(currentLevel);
            initScreenPlot();
            return Q_HANDLED();
        }
        case ChangeLevelDown: {
            xil_printf("change mode down\n");
            currentLevel = (currentLevel == 0) ? 2 : currentLevel - 1;
            changeLevel(currentLevel);
            initScreenPlot();
            return Q_HANDLED();
        }
        case GameOn: {
            xil_printf("game on\n");
            setBallInitPosition();
            return Q_TRAN(&GameRunning);
        }
    }
    return Q_SUPER(&QHsm_top);
}

QState GameRunning(GemeOn *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
			return Q_HANDLED();
		}
        case TICK_SIG: {
			calculateNewPosition();
            changePosition();
            if(calculateHit()){
                return Q_TRAN(&GameUpdate);
            }
			return Q_HANDLED();
		}
        case BoardsChange: {
            plotBoards();
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}

QState GameUpdate(GemeOn *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            calculateDirection();
            if(calculateIsOver()){
                updateScore();
                return Q_TRAN(&GameRunning);
            }else{
                return Q_TRAN(&GameOver);
            }
			return Q_HANDLED();
		}
        case ChangeStatus: {
            // preserved state, can be adding balls random obstacle
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}

QState GameOver(GemeOn *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            displayEnd();
            return Q_HANDLED();
        }
        case GameOn: {
            return Q_TRAN(&GameOn_start);
        }
    }
    return Q_SUPER(&QHsm_top);
}

// set level 0 easy, 1 medium, 2 difficult
int changeLevel(int level){
    // set level parameter and other stuff
}

// plot init frame (choose level and start game)
void initScreenPlot(int level){
    // plot screen
}

// set ball's init position
void setBallInitPosition(){
    //
}

//calculate next position
void calculateNewPosition(){

}

// plot ball's new position
void changePosition(){

}

// calculate if hit the boarder
int calculateHit(){

}

// plot reflect boards in the bottom according to the position switch
int plotBoards(){

}

//calculate new direction after reflection
void calculateDirection(){

}

// calculate if the game is over 
int calculateIsOver(){

}
            
//calculate if scored
void updateScore(){

}

// display game over
void displayEnd(){

}
