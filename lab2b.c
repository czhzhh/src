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
#include "logic.h"
#include <stdlib.h>
#include <time.h>
#include <float.h>

typedef struct Lab2BTag  {               //Lab2B State machine
	QActive super;
} Lab2B;

 volatile int d=0;


int currentMode = 1;
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
            score = 0;
            int vx = 6;
            int vy = 8;
            initBall(&ball, 220, 160, vx, vy, 5);
            return Q_HANDLED();
        }
        case TICK_SIG: {
            return Q_HANDLED();
        }
        case ChangeLevelUp: {
            xil_printf("change mode up\n\r");
            initPlotVar = !initPlotVar;
            draw_arrow_init();
            return Q_HANDLED();
        }
        case ChangeLevelDown: {
            xil_printf("change mode down\n\r");
            initPlotVar = !initPlotVar;
            draw_arrow_init();
            return Q_HANDLED();
        }
        case GameOn: {
            xil_printf("game on\n\r");
            if(initPlotVar == 0){
                if(currentMode){
                	Game_Init();
                    return Q_TRAN(&fsm_SW);
                }else{
                	Game_Init();
                	btn_init_game();
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
            setChangeFlag = (setChangeFlag == 0) ? 4 : setChangeFlag - 1;
            draw_arrow_settings();
            return Q_HANDLED();
        }
        case ChangeLevelDown: {
            xil_printf("change mode down\n\r");
            setChangeFlag = (setChangeFlag == 4) ? 0 : setChangeFlag + 1;
            draw_arrow_settings();
            return Q_HANDLED();
        }
        case GameOn: {
            xil_printf("back to begining\n\r");
            dspl_init();
            return Q_TRAN(&fsm_on_start);
        }
        case B_L: {
            // change paras according to setChangeFlag
            downbtn_setting_change(&ball, moving_step);
			return Q_HANDLED();
		}
        case B_R: {
            // change paras according to setChangeFlag
            upbtn_setting_change(&ball, moving_step);
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
        	if(ball.x <= boarder.x_min){
				if(calculateReflect(ball.y, now_yleft, y_bias, Brck_Pos, valid_sw, currentMode, score)){
					//return Q_TRAN(&fsm_over);
				}
				score += 5*(6-count);
				regenerateSpeed(&ball.vx, &ball.vy);
				xil_printf("ball vx in SW%d",ball.vx);
			}
            update();
			return Q_HANDLED();
		}
        case BoardsChange: {
        	xil_printf("message sent\r\n");
            Bricks(count,positions);
            return Q_HANDLED();
        }
        case GameOn: {

            return Q_HANDLED();
        }
        case ChangeLevelUp: {
            return Q_HANDLED();
        }
        case ChangeLevelDown: {
            return Q_HANDLED();
        }
        case B_L: {
			return Q_HANDLED();
		}
        case B_R: {
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
        	if(ball.x <= boarder.x_min){
        		if(calculateReflect(ball.y, now_yleft, y_bias, Brck_Pos, valid_sw, currentMode, score)){
					return Q_TRAN(&fsm_over);
				}
        		score += 10;
        		regenerateSpeed(&ball.vx, &ball.vy);
        	}
            update();
			return Q_HANDLED();
		}
        case BoardsChange: {
            return Q_HANDLED();
        }
        case GameOn: {
            return Q_HANDLED();
        }
        case ChangeLevelUp: {
            return Q_HANDLED();
        }
        case ChangeLevelDown: {
            return Q_HANDLED();
        }
        case B_L: {
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
            dspl_end();
            return Q_HANDLED();
        }
        case GameOn: {
        	clrScr();
        	score = 0;
        	int vx = 6;
        	int vy = 8;
        	initBall(&ball, 220, 160, vx, vy, 5);
        	dspl_init();
            return Q_TRAN(&fsm_on_start);
        }
        case TICK_SIG: {
            return Q_HANDLED();
        }
        case BoardsChange: {
            return Q_HANDLED();
        }
        case ChangeLevelUp: {
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



