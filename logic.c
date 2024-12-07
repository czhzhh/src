
#include <stdio.h>
#include <math.h>
#include "qpn_port.h"
#include "bsp.h"
#include "lab2b.h"
#include "xintc.h"
#include "xil_exception.h"
#include "xparameters.h"
#include "xil_cache.h"
#include "xtmrctr.h"
#include "xtmrctr_l.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "xspi.h"
#include "xspi_l.h"
#include "lcd.h"
#include <stdio.h>
#include "logic.h"
#include "lab2b.h"

#define MAX_RADIUS 20

void initBall(Ball *ball, int x, int y, int vx, int vy, int radius) {
	if (radius < 1 || radius > MAX_RADIUS) return; //check the validation of r
    ball->x = x;
	ball->y = y;
    ball->vx = vx;
    ball->vy = vy;
    ball->radius = radius;
}

void set_boarder(Ball *ball, Boarder *Boarder){
	Boarder->x_min=10 +ball->radius;//8 is the thickness of bricks
	Boarder->x_max=239-ball->radius;
	Boarder->y_min=    ball->radius;
	Boarder->y_max=319-ball->radius;
}
void updateBall(Ball *ball, Boarder *Boarder) {

    ball->x += ball->vx;
    ball->y += ball->vy;

    if (ball->x <= Boarder->x_min) {
        ball->x = Boarder->x_min;
        ball->vx = -ball->vx;
    } else if (ball->x >= Boarder->x_max) {
        ball->x = Boarder->x_max;
        ball->vx = -ball->vx;
    }
    if (ball->y <= Boarder->y_min) {
        ball->y = Boarder->y_min;
        ball->vy = -ball->vy;
    } else if (ball->y >= Boarder->y_max) {
        ball->y = Boarder->y_max;
        ball->vy = -ball->vy;
    }
}

/*
 * Use this function in updateBall when you need to update the speed of x
 * return 0: game continues
 * return 1: game over
 */
int calculateReflect(int currentPositionY, int now_yleft, int y_bias, int Brck_Pos[][4], int BRICKS_COUNT, int currentMode) {
    if (!currentMode) {
    	xil_printf("currentPositionY %d",currentPositionY);
    	xil_printf("now_yleft %d",now_yleft);
    	xil_printf("now_yleft+ y_bias %d",now_yleft+ y_bias);
        return (currentPositionY >= now_yleft && currentPositionY <= now_yleft + y_bias) ? 0 : 1;
    } else {
        int returnValue = 1;
//        for (int i = 0; i < BRICKS_COUNT; i++) {
//        	xil_printf("%d",Brck_Pos[i][1]);
//            if (currentPositionY >= Brck_Pos[i][1] && currentPositionY <= Brck_Pos[i][3]) {
//                returnValue = 0;
//                break;
//            }
//        }
        if (count > 0) {
			for (int i = 0; i < count; i++) {
				int index = positions[i];
				//xil_printf("index %d\n\r",index);
				xil_printf("currentPositionY %d\n\r",currentPositionY);
				xil_printf("Brck_Pos[index][1] %d\n\r",Brck_Pos[index][1]);
				xil_printf("Brck_Pos[index][3] %d\n\r",Brck_Pos[index][3]);
				if (currentPositionY >= Brck_Pos[index][1] && currentPositionY <= Brck_Pos[index][3]) {
					returnValue = 0;
					break;
				}
			}
		}
        return returnValue;
    }
}


