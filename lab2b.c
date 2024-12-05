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

static QState fsm_initial (Lab2B *me);
// static QState fsm_game (Lab2B *me);
static QState fsm_on_start (Lab2B *me);
static QState fsm_running (Lab2B *me);
static QState fsm_update (Lab2B *me);
static QState fsm_over (Lab2B *me);
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
            // plot init frame(choose level and start game)
            initScreenPlot();
            return Q_HANDLED();
        }
        case TICK_SIG: {
            return Q_HANDLED();
        }
        case ChangeLevelUp: {
            xil_printf("change mode up\n\r");
            currentLevel = (currentLevel == 2) ? 0 : currentLevel + 1;
            changeLevel(currentLevel);
            initScreenPlot();
            return Q_HANDLED();
        }
        case ChangeLevelDown: {
            xil_printf("change mode down\n\r");
            currentLevel = (currentLevel == 0) ? 2 : currentLevel - 1;
            changeLevel(currentLevel);
            initScreenPlot();
            return Q_HANDLED();
        }
        case GameOn: {
            xil_printf("game on\n\r");
            setBallInitPosition();
            return Q_TRAN(&fsm_running);
        }
        case BoardsChange: {
            return Q_HANDLED();
        }
        case ChangeStatus: {
            return Q_HANDLED();
        }
    }
	return Q_SUPER(&QHsm_top);
}
// void Vol_up(){
// 	printf("Volume: %d\n",act_volume);
// 	setColor(0, 0, 0);
// 	fillRect(act_volume+70, 90, 170, 110);
// 	setColorBg(255, 0, 0);
// 	fillRect(act_volume+69, 90, act_volume+70, 110);
// 	MainVolumeState=1;
// 	VolumeState=1;
// }
// void Vol_down(){
// 	printf("Volume: %d\n",act_volume);
// 	setColor(0, 0, 0);
// 	fillRect(act_volume+71, 90, 170, 110);
// 	setColor(0, 0, 0);
// 	fillRect(act_volume+70, 90, act_volume+71, 110);
// 	MainVolumeState = 1;
// 	VolumeState = 1;
// }

// void VolumeToggle(int toggle) {
// 	if (toggle == 1) {
// 		last_vol=act_volume;
// 		act_volume=0;
// 		setColor(0, 0, 0);
// 		fillRect(70, 90, 170, 110);
// 		MainVolumeState = 1;
// 		VolumeState = 1;
// 	} 
// 	else {
// 		act_volume=last_vol;
// 		setColor(0, 255, 0);
// 		fillRect(69, 90, 70+act_volume, 110);
// 		setColor(0, 0, 0);
// 		fillRect(act_volume+71, 90, 170, 110);
// 		MainVolumeState=1;
// 		VolumeState=1;
// 	}
// }
// void DisplText(char* s1) {
// 	Txt_Set_BLUE(); 
// 	setColor(255, 191, 50);
// 	setColorBg(255, 255, 255);
// 	setFont(BigFont);
// 	lcdPrint(s1, 75, 140);
// 	MainTextState = 1;
// 	TextState = 1;
// }
/* Create Lab2B_on state and do any initialization code if needed */
/******************************************************************/
QState fsm_running(Lab2B *me) {
	    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
        	xil_printf("fsm_running\n\r");
			return Q_HANDLED();
		}
        case TICK_SIG: {
			calculateNewPosition();
            changePosition();
            if(calculateHit()){
            	xil_printf("do_update\n\r");
				calculateDirection();
				if(calculateIsOver()){
					updateScore();
				}else{
					xil_printf("do_update over\n\r");
					return Q_TRAN(&fsm_over);
				}
            }
			return Q_HANDLED();
		}
        case BoardsChange: {
            plotBoards();
            return Q_HANDLED();
        }
        case GameOn: {
            return Q_HANDLED();
        }
        case ChangeLevelUp: {
        	// delate this later
        	xil_printf("running to over\n\r");
        	update();
        	d+=1;
        	return Q_TRAN(&fsm_over);
            //return Q_HANDLED();
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
/*
QState fsm_update(Lab2B *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
        	xil_printf("fsm_update\n\r");
            calculateDirection();
            if(calculateIsOver()){
                updateScore();
                xil_printf("fsm_update 1\n\r");
                return Q_TRAN(&fsm_running);
            }else{
            	xil_printf("fsm_updatem 2\n\r");
                return Q_TRAN(&fsm_over);
            }
			return Q_HANDLED();
		}
        case ChangeStatus: {
            // preserved state, can be adding balls random obstacle
            return Q_HANDLED();
        }
        case BoardsChange: {
            plotBoards();
            return Q_HANDLED();
        }
        case TICK_SIG: {
            return Q_HANDLED();
        }
        case GameOn: {
            return Q_HANDLED();
        }
        case ChangeLevelUp: {
        	// delate this later
        	xil_printf("ChangeLevelUp in fsm_update\n\r");
        	return Q_TRAN(&fsm_over);
        	//return Q_HANDLED();
        }
        case ChangeLevelDown: {
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&fsm_on_start);
}
*/
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
    }
    return Q_SUPER(&fsm_on_start);
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
