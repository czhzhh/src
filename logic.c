
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

void upbtn_setting_change(Ball *ball, int moving_step){
	if(setChangeFlag==0){
		if(currentMode){
			//switch
		}
		else{
			//board
		}
	}
	else if(setChangeFlag==1){
		valid_sw = (valid_sw == 8) ? 8 : valid_sw + 1;
	}
	else if(setChangeFlag==2){
		ball->radius=(ball->radius==MAX_RADIUS) ? MAX_RADIUS : ball->radius+1;
	}
	else if(setChangeFlag==3){
		//v=(v<=MAX_V) ? MAX_V : v+1;
	}
	else if(setChangeFlag==4){
		moving_step=(moving_step==30) ? 30 : moving_step+1;
		DisplInt(moving_step            , 1,100,200, SmallFont);
	}
//	DisplText("Game mode"           , 1,180,60, SmallFont);	 DisplText("Bricks or Board"     , 1,180,200, SmallFont);
//	DisplText("Bricks(max 8)"       , 1,160,60, SmallFont);	 DisplInt(valid_sw               , 1,160,200, SmallFont);
//	DisplText("Bullet size"         , 1,140,60, SmallFont);  DisplInt(ball.radius            , 1,140,200, SmallFont);
//	DisplText("Bullet Velocity"     , 1,120,60, SmallFont);  DisplInt(bullet_velocity        , 1,120,200, SmallFont);
//	DisplText("Board Velocity"      , 1,100,60, SmallFont);
}

void downbtn_setting_change(Ball *ball, int moving_step){
	if(setChangeFlag==0){
		if(currentMode){
			//switch
		}
		else{
			//board
		}
	}
	else if(setChangeFlag==1){
		valid_sw = (valid_sw == 1) ? 1 : valid_sw - 1;
	}
	else if(setChangeFlag==2){
		ball->radius=(ball->radius==3) ? 3 : ball->radius-1;
	}
	else if(setChangeFlag==3){
		//v=(v<=MIN_V) ? MIN_V : v-1;
	}
	else if(setChangeFlag==4){
		moving_step=(moving_step==5) ? 5 : moving_step-1;
	}
}


/*
 * Use this function in updateBall when you need to update the speed of x
 * return 0: game continues
 * return 1: game over
 */
int calculateReflect(int currentPositionY, int now_yleft, int y_bias, int Brck_Pos[][4], int BRICKS_COUNT, int currentMode) {
    if (!currentMode) {
        // change vx vy when game continues
        return (currentPositionY >= now_yleft && currentPositionY <= now_yleft + y_bias) ? 0 : 1;
    } else {
        int returnValue = 1;
        if (count > 0) {
			for (int i = 0; i < count; i++) {
				int index = positions[i];
				if (currentPositionY >= Brck_Pos[index][1] && currentPositionY <= Brck_Pos[index][3]) {
                    // change vx vy
					returnValue = 0;
					break;
				}
			}
		}
        return returnValue;
    }
}
