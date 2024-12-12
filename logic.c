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
#include <float.h>

#define MAX_RADIUS 10
#define MAX_V 30
#define MIN_V 10
VelocityArray vel_array = {0};
Obstacle randomObstacles[5];
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

int is_between(double value, double bound1, double bound2) {
    double min = (bound1 < bound2) ? bound1 : bound2;
    double max = (bound1 > bound2) ? bound1 : bound2;
    return (value >= min && value <= max);
}


void get_intersection(Ball *ball,int p1_x, int p1_y, int p2_x, int p2_y, int x_min, int x_max, int y_min, int y_max) {
    // Calculate the direction vector of the line
    double dx = p2_x - p1_x;
    double dy = p2_y - p1_y;

    // Intersection points
    double x_intersect = 0, y_intersect = 0;
    double t_min = DBL_MAX;

    // Check left edge intersection
    if (dx != 0) {
        double t_left = (x_min - p1_x) / dx;
        if (t_left >= 0 && t_left <= 1) {
            y_intersect = p1_y + t_left * dy;
            if (is_between(y_intersect, y_min, y_max)) {
                if (t_left < t_min) {
                    t_min = t_left;
                    ball->vx = -ball->vx;
                    xil_printf("xmin\r\n");
                    x_intersect = x_min;
                }
            }
        }
    }

    // Check right edge intersection
    if (dx != 0) {
        double t_right = (x_max - p1_x) / dx;
        if (t_right >= 0 && t_right <= 1) {
            y_intersect = p1_y + t_right * dy;
            if (is_between(y_intersect, y_min, y_max)) {
                if (t_right < t_min) {
                    t_min = t_right;
                    ball->vx = -ball->vx;
                    xil_printf("xmax\r\n");
                    x_intersect = x_max;
                }
            }
        }
    }

    // Check bottom edge intersection
    if (dy != 0) {
        double t_bottom = (y_min - p1_y) / dy;
        if (t_bottom >= 0 && t_bottom <= 1) {
            x_intersect = p1_x + t_bottom * dx;
            if (is_between(x_intersect, x_min, x_max)) {
                if (t_bottom < t_min) {
                    t_min = t_bottom;
                    ball->vy = -ball->vy;
                    xil_printf("ymin\r\n");
                    y_intersect = y_min;
                }
            }
        }
    }

    // Check top edge intersection
    if (dy != 0) {
        double t_top = (y_max - p1_y) / dy;
        if (t_top >= 0 && t_top <= 1) {
            x_intersect = p1_x + t_top * dx;
            if (is_between(x_intersect, x_min, x_max)) {
                if (t_top < t_min) {
                    t_min = t_top;
                    ball->vy = -ball->vy;
                    xil_printf("ymax\r\n");
                    y_intersect = y_max;
                }
            }
        }
    }
}

void updateBall(Ball *ball, Boarder *Boarder) {

    ball->x += ball->vx;
    ball->y += ball->vy;
//    for (int i = 0; i < 5; i++) {
//            if (randomObstacles[i].enable) {
//                if (ball->x >= randomObstacles[i].x_min+ball->radius && ball->x <= randomObstacles[i].x_max+ball->radius
//                		&& ball->y >= randomObstacles[i].y_min+ball->radius && ball->y <= randomObstacles[i].y_max+ball->radius) {
//                	xil_printf("randomObstacles %d\r\n",i);
//                	get_intersection(ball, ball->x- ball->vx, ball->y - ball->vy, ball->x, ball->y,
//                			randomObstacles[i].x_min+ball->radius, randomObstacles[i].x_max+ball->radius
//							, randomObstacles[i].y_min+ball->radius, randomObstacles[i].y_max+ball->radius);
//                }
//            }
//        }
    if (ball->x <= Boarder->x_min) {
        ball->x = Boarder->x_min;
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

void upbtn_setting_change(int* currentMode,Ball *ball, int* moving_step,int *bullet_velocity, int *y_bias){
	if(setChangeFlag==0){
		*currentMode = (*currentMode == 0) ? 1 : 0;//board
		printf("%d",*currentMode);
		dspl_Br_Bo();
	}
	else if(setChangeFlag==1){
		valid_sw = (valid_sw == 8) ? 8 : valid_sw + 1;
		DisplInt(valid_sw      , 1,160,200, SmallFont);
	}
	else if(setChangeFlag==2){
		ball->radius=(ball->radius==MAX_RADIUS) ? MAX_RADIUS : ball->radius+1;
		DisplInt(ball->radius  , 1,140,200, SmallFont);
	}
	else if(setChangeFlag==3){
		*bullet_velocity = (*bullet_velocity==MAX_V) ? MAX_V : *bullet_velocity+1;
		DisplInt(*bullet_velocity, 1,120,200, SmallFont);
	}
	else if(setChangeFlag==4){
		*moving_step=(*moving_step==30) ? 30 : *moving_step+1;
		DisplText("   "          , 1,100,200, SmallFont);
		DisplInt(*moving_step    , 1,100,200, SmallFont);
	}
	else if(setChangeFlag==5){
		*y_bias=(*y_bias==200) ? 200 : *y_bias+10;
		DisplText("    "         , 1, 80,200, SmallFont);
		DisplInt(*y_bias         , 1, 80,200, SmallFont);
	}
}

void downbtn_setting_change(int* currentMode,Ball *ball, int* moving_step,int *bullet_velocity, int *y_bias){
	if(setChangeFlag==0){
		*currentMode = (*currentMode == 0) ? 1 : 0;//board
		dspl_Br_Bo();
	}
	else if(setChangeFlag==1){
		valid_sw = (valid_sw == 1) ? 1 : valid_sw - 1;
		DisplInt(valid_sw      , 1,160,200, SmallFont);
	}
	else if(setChangeFlag==2){
		ball->radius=(ball->radius==3) ? 3 : ball->radius-1;
		DisplText("    "         , 1, 140,200, SmallFont);
		DisplInt(ball->radius  , 1,140,200, SmallFont);
	}
	else if(setChangeFlag==3){
		*bullet_velocity = (*bullet_velocity==MIN_V) ? MIN_V : *bullet_velocity-1;
		DisplInt(*bullet_velocity, 1,120,200, SmallFont);
	}
	else if(setChangeFlag==4){
		*moving_step=(*moving_step==5) ? 5 : *moving_step-1;
		DisplText("   "          , 1,100,200, SmallFont);
		DisplInt(*moving_step    , 1,100,200, SmallFont);
	}
	else if(setChangeFlag==5){
		*y_bias=(*y_bias==20) ? 20 : *y_bias-10;
		DisplText("    "         , 1, 80,200, SmallFont);
		DisplInt(*y_bias         , 1, 80,200, SmallFont);
	}
}

int calculateReflect(int currentPositionY, int now_yleft, int y_bias, int Brck_Pos[][4], int BRICKS_COUNT, int currentMode, int score, int ball_radius) {
    if (!currentMode) {
    	// score updated in lab2b.c
        return (currentPositionY >= (now_yleft - ball_radius) && currentPositionY <= (now_yleft + y_bias + ball_radius)) ? 0 : 1;
    } else {
        int returnValue = 1;
        if (count > 0) {
			for (int i = 0; i < count; i++) {
				int index = positions[i];
				if (currentPositionY >= (Brck_Pos[index][1]- ball_radius) && currentPositionY <= (Brck_Pos[index][3]+ball_radius)) {
					returnValue = 0;
					break;
				}
			}
		}
        return returnValue;
    }
}

void generateSpeed(int *vx, int *vy){
	double speed_squared = (double)(*vx * *vx + *vy * *vy);
	printf("speed_squared %f\r\n",speed_squared);
	double speed = sqrt(speed_squared);
	double max_bias = 0.30 * fabs(*vx);
	double x = rand()%10;
	double bias = (2*x/10-1) * max_bias;
	printf("and()10 %f\r\n",x);
	printf("bias %f\r\n",bias);
	double vx1 = (double)*vx;
	double vy1 = (double)*vy;
	vx1 += bias;
	printf("vx1 %f\r\n",vx1);
	double vx_squared = vx1 * vx1;
	if (vx_squared > speed_squared) {
		vx1 = copysign(speed, vx1);
		vx_squared = speed_squared - 1;
	}
	double vy_squared = speed_squared - vx_squared;
	//vy_squared = fmax(vy_squared, 25);
	vy_squared = fmax(vy_squared, speed_squared*0.25);
	vy1 = copysign(sqrt(vy_squared), vy1);
	*vy = (int)lround(vy1);
//	*vx = -(int)fmin(lround(vx1), -5.0);
	printf("speed %f\r\n",-speed/2);
	printf("vx1 %f\r\n",vx1);
	*vx = -(int)fmin(lround(vx1), -speed/2);
}

void init_velocities(int initial_v) {
    float vel_seed[6] = {0.42857, 0.57142,
                         0.33333, 0.66666,
                         0.5    , 0.5};

    int seed = rand() % 3;
    int current_vx = initial_v * vel_seed[seed*2];
    int current_vy = initial_v * vel_seed[seed*2+1];
    vel_array.vx[0] = current_vx;
    vel_array.vy[0] = current_vy;
    for (int i = 1; i < MAX_VELOCITIES; i++) {
    	current_vx=-vel_array.vx[i-1];
    	current_vy=vel_array.vy[i-1];
        generateSpeed(&current_vx, &current_vy);
        vel_array.vx[i] = current_vx;
        vel_array.vy[i] = current_vy;
        printf("Velocity %d: (%d,%d)\n", i, current_vx, current_vy);
    }
    vel_array.current_index = 0;
}

void updateBallSpeed(Ball* ball) {
    ball->vx = vel_array.vx[vel_array.current_index];
    printf("%d\r\n",ball->vx);
    ball->vy = copysign(vel_array.vy[vel_array.current_index], ball->vy);
    vel_array.current_index = (vel_array.current_index + 1) % MAX_VELOCITIES;
}
