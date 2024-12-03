/*****************************************************************************
* lab2B.c for Lab2B of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/

#include "qpn_port.h"
#include "bsp.h"
#include "gemeOn.h"
#include "lcd.h"
#include <math.h>

typedef struct GameOnTag  {               //Lab2B State machine
	QActive super;
} GemeOn;

int currentLevel = 0;

static QState GameOn_initial (GemeOn *me);
// static QState GameOn_on      (gemeOn *me);
// static QState Lab2B_A  (gemeOn *me);
// static QState Lab2B_B  (gemeOn *me);

GemeOn newGame;

void GemeOn_ctor(void)  {
	GemeOn *me = &newGame;
	QActive_ctor(&me->super, (QStateHandler)&GemeOn_initial);
}

QState GemeOn_initial(GemeOn *me) {
    xil_printf("\n\rInitialization");
    return Q_TRAN(&GameOn_start);
}

QState GameOn_start(GemeOn *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            xil_printf("rendering init screen\n");
            // 设置初始难度
            changeLevel(currentLevel);
            // 绘制初始界面
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
            //预留的一个状态，可以往里加球裂变、随机障碍等功能
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}

QState GameOver(GemeOn *me) {
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            displayEnd();
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

// 绘制初始界面 
void initScreenPlot(int level){
    // plot screen
}

// 设置小球初始位置
void setBallInitPosition(){
    //
}

// 计算新的位置
void calculateNewPosition(){

}

// 画出现在的小球位置
void changePosition(){

}

// 计算是否击中边缘
int calculateHit(){

}

// 根据switch绘制底部反弹板
int plotBoards(){

}

// 计算反弹后的斜率
void calculateDirection(){

}

// 计算是否结束游戏
int calculateIsOver(){

}
            
// 计算当前得分
void updateScore(){

}

// 画出结束画面
void displayEnd(){

}