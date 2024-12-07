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
            // set init
            // vx = rand(bullet_velocity)
            int vx = 6;
            int vy = 8;
            initBall(&ball, 220, 160, vx, vy, 5);
            // plot init frame(choose level and start game)
            dspl_init();
            return Q_HANDLED();
        }
        case TICK_SIG: {
            return Q_HANDLED();
        }
        case ChangeLevelUp: {
            xil_printf("change mode up\n\r");
            initPlotVar = !initPlotVar;
            //dspl_init();
            return Q_HANDLED();
        }
        case ChangeLevelDown: {
            xil_printf("change mode down\n\r");
            initPlotVar = !initPlotVar;
            //dspl_init();
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
            setChangeFlag = (setChangeFlag == 4) ? 0 : setChangeFlag + 1;
            // plot the arrow
            return Q_HANDLED();
        }
        case ChangeLevelDown: {
            xil_printf("change mode down\n\r");
            setChangeFlag = (setChangeFlag == 0) ? 4 : setChangeFlag - 1;
            // plot the arrow
            return Q_HANDLED();
        }
        case GameOn: {
            xil_printf("back to begining\n\r");
            return Q_TRAN(&fsm_on_start);
        }
        case B_L: {
            // change paras according to setChangeFlag
        	switch(setChangeFlag){
        	case 0:
        		currentMode!=currentMode;
        	case 1:
        		valid_sw = (valid_sw == 1) ? 1 : valid_sw - 1;
        	case 2:
        		ball.radius = (ball.radius == 1) ? 1 : ball.radius - 1;
        	case 3:
        		bullet_velocity = (bullet_velocity == 5) ? 5 : bullet_velocity - 1;
        	case 4:
        		moving_step = (moving_step == 5) ? 5 : moving_step - 1;
        	}
			return Q_HANDLED();
		}
        case B_R: {
            // change paras according to setChangeFlag
        	switch(setChangeFlag){
        	case 0:
        		currentMode!=currentMode;
        	case 1:
        		valid_sw = (valid_sw == 8) ? 8 : valid_sw + 1;
        	case 2:
        		ball.radius = (ball.radius == 20) ? 20 : ball.radius + 1;
        	case 3:
        		bullet_velocity = (bullet_velocity == 20) ? 20 : bullet_velocity + 1;
        	case 4:
        		moving_step = (moving_step == 20) ? 20 : moving_step + 1;
        	}
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
        	if(ball.x <= boarder.x_min){
				if(calculateReflect(ball.y, now_yleft, y_bias, Brck_Pos[16][4], valid_sw, currentMode)){
					return Q_TRAN(&fsm_over);
				}
			}
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
        		if(calculateReflect(ball.y, now_yleft, y_bias, Brck_Pos[16][4], valid_sw, currentMode)){
					return Q_TRAN(&fsm_over);
				}
        	}
            update();
            // int gameover
            // if(gameover){
            // 	xil_printf("game over\n\r");
			// 	return Q_TRAN(&fsm_over);
            // }
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
            //displayEnd();
            return Q_HANDLED();
        }
        case GameOn: {
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



