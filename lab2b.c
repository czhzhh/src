/*****************************************************************************
* lab2B.c for Lab2B of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/

#include "qpn_port.h"
#include "bsp.h"
#include "lab2b.h"
#include "lcd.h"
#include "displayer.h"
#include <math.h>
#include "xil_printf.h"



typedef struct Lab2BTag  {               //Lab2B State machine
	QActive super;
} Lab2B;

 int act_volume = 0;
 int stored_value = 0;
 int Mute = 0;
 int MainVolumeState = 0;
 int VolumeState = 0;
 int MainTextState = 0;
 int TextState = 0;
 int last_vol=0;
 volatile int d=0;

int currentLevel = 0;
int currentMode = 0;
int initPlotVar = 0;
int setChangeFlag = 0;

static QState fsm_initial (Lab2B *me);
static QState fsm_on_start (Lab2B *me);
static QState fsm_SW (Lab2B *me);
static QState fsm_Btn (Lab2B *me);
static QState fsm_over (Lab2B *me);
static QState fsm_Setting (Lab2B *me);
/**********************************************************************/
Lab2B l2b;


void Lab2B_ctor(void)  {
	Lab2B *me = &l2b;
	QActive_ctor(&me->super, (QStateHandler)&fsm_initial);
}


QState fsm_initial(Lab2B *me) {
	xil_printf("Initialization\n\r");
    return Q_TRAN(&fsm_on_start);
}

QState fsm_on_start(Lab2B *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            xil_printf("rendering init screen fsm_on_start\n\r");
            // set init level
            changeLevel(currentLevel);
            setMode(currentMode);
            // plot init frame(choose level and start game)
            initScreenPlot();
            return Q_HANDLED();
        }
        case TICK_SIG: {
            return Q_HANDLED();
        }
        case ChangeLevelUp: {
            xil_printf("change mode up\n\r");
            initPlotVar = (initPlotVar == 1) ? 0 : initPlotVar + 1;
            initScreenPlot();
            return Q_HANDLED();
        }
        case ChangeLevelDown: {
            xil_printf("change mode down\n\r");
            // currentLevel = (currentLevel == 0) ? 2 : currentLevel - 1;
            // changeLevel(currentLevel);
            initPlotVar = (initPlotVar == 1) ? 0 : initPlotVar + 1;
            initScreenPlot();
            return Q_HANDLED();
        }
        case GameOn: {
            xil_printf("game on\n\r");
            //setBallInitPosition();
            if(initPlotVar == 0){
                if(currentMode){
                    return Q_TRAN(&fsm_SW);
                }else{
                    return Q_TRAN(&fsm_Btn);
                }
            }else{
                return Q_TRAN(&fsm_Setting);
            }
            return Q_HANDLED();
        }
        case BoardsChange: {
            return Q_HANDLED();
        }
        case ChangeStatus: {
            return Q_HANDLED();
        }
        case B_L: {
			return Q_HANDLED();
		}
        case B_R: {
			return Q_HANDLED();
		}
    }
	return Q_SUPER(&QHsm_top);
}

QState fsm_Setting(Lab2B *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            xil_printf("Into settinhg\n\r");
            // set init level
            dspl_Settings();
            return Q_HANDLED();
        }
        case TICK_SIG: {
            return Q_HANDLED();
        }
        case ChangeLevelUp: {
            xil_printf("change mode up\n\r");
            setChangeFlag = (setChangeFlag == 4) ? 0 : setChangeFlag + 1;
            // plot the arrow
            return Q_HANDLED();
        }
        case ChangeLevelDown: {
            xil_printf("change mode down\n\r");
            setChangeFlag = (setChangeFlag == 4) ? 0 : setChangeFlag - 1;
            // plot the arrow
            return Q_HANDLED();
        }
        case GameOn: {
            xil_printf("back to begining\n\r");
            return Q_TRAN(&fsm_on_start);
        }
        case B_L: {
            // change paras according to setChangeFlag
			return Q_HANDLED();
		}
        case B_R: {
            // change paras according to setChangeFlag
			return Q_HANDLED();
		}
        case BoardsChange: {
            return Q_HANDLED();
        }
        case ChangeStatus: {
            return Q_HANDLED();
        }
    }
	return Q_SUPER(&fsm_on_start);
}

QState fsm_SW(Lab2B *me) {
	    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
        	xil_printf("fsm_SW\n\r");
			return Q_HANDLED();
		}
        case TICK_SIG: {
			// calculateNewPosition();
            // changePosition();
            update();
            // int gameover
            // if(gameover){
            // 	xil_printf("game over\n\r");
			// 	return Q_TRAN(&fsm_over);

            // }
			return Q_HANDLED();
		}
        case BoardsChange: {
            Bricks(count,positions);
            free_positions();
            // plotBoards();
            return Q_HANDLED();
        }
        case GameOn: {
            return Q_HANDLED();
        }
        case ChangeLevelUp: {
        	// delate this later
        	// update();
        	// d+=1;
        	// return Q_TRAN(&fsm_over);
            return Q_HANDLED();
        }
        case ChangeLevelDown: {
        	xil_printf("down button pressed\n");
        	//dspl_game_Init();
            return Q_HANDLED();
        }
        case B_L: {
			//Bricks(count, positions);
			//free_positions();
        	//btn_mov_l();
			return Q_HANDLED();
		}
        case B_R: {
			//btn_mov_r();
			return Q_HANDLED();
		}
        case ChangeStatus: {
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&fsm_on_start);
}

QState fsm_Btn(Lab2B *me) {
	    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
        	xil_printf("fsm_Btn\n\r");
			return Q_HANDLED();
		}
        case TICK_SIG: {
			// calculateNewPosition();
            // changePosition();
            update();
            // int gameover
            // if(gameover){
            // 	xil_printf("game over\n\r");
			// 	return Q_TRAN(&fsm_over);

            // }
			return Q_HANDLED();
		}
        case BoardsChange: {
            //Bricks(count,positions);
            //free_positions();
            // plotBoards();
            return Q_HANDLED();
        }
        case GameOn: {
            return Q_HANDLED();
        }
        case ChangeLevelUp: {
        	// delate this later
        	// update();
        	// d+=1;
        	// return Q_TRAN(&fsm_over);
            return Q_HANDLED();
        }
        case ChangeLevelDown: {
        	xil_printf("down button pressed\n");
        	//dspl_game_Init();
            return Q_HANDLED();
        }
        case B_L: {
			//Bricks(count, positions);
			//free_positions();
        	btn_mov_l();
			return Q_HANDLED();
		}
        case B_R: {
			btn_mov_r();
			return Q_HANDLED();
		}
        case ChangeStatus: {
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&fsm_on_start);
}

QState fsm_over(Lab2B *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
        	xil_printf("fsm_over\n\r");
            displayEnd();
            return Q_HANDLED();
        }
        case GameOn: {
        	xil_printf("rendering init screen from fsm_over\n\r");
			// set init level
			changeLevel(currentLevel);
			// plot init frame(choose level and start game)
			initScreenPlot();
            return Q_TRAN(&fsm_on_start);
        }
        case TICK_SIG: {
            return Q_HANDLED();
        }
        case BoardsChange: {
            return Q_HANDLED();
        }
        case ChangeLevelUp: {
        	xil_printf("ChangeLevelUp in fsm_over\n\r");
            return Q_HANDLED();
        }
        case ChangeLevelDown: {
            return Q_HANDLED();
        }
        case ChangeStatus: {
            return Q_HANDLED();
        }
        case B_L: {
			return Q_HANDLED();
		}
        case B_R: {
			return Q_HANDLED();
		}
    }
    return Q_SUPER(&fsm_on_start);
}

// set level 0 easy, 1 medium, 2 difficult
int changeLevel(int level){
    // set level parameter and other stuff
}

// set level 0 easy, 1 medium, 2 difficult
int setMode(int currentMode){
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
	return 1;
}

// plot reflect boards in the bottom according to the position switch
int plotBoards(){

}

//calculate new direction after reflection
void calculateDirection(){

}

// calculate if the game is over 
int calculateIsOver(){
	return 0;
}
            
//calculate if scored
void updateScore(){

}

// display game over
void displayEnd(){

}
