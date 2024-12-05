
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
	printf("x_min: %d\n", Boarder->x_min);
	    printf("x_max: %d\n", Boarder->x_max);
	    printf("y_min: %d\n", Boarder->y_min);
	    printf("y_max: %d\n", Boarder->y_max);
}
void updateBall(Ball *ball, Boarder *Boarder) {
    // �ȸ���λ��
    ball->x += ball->vx;
    ball->y += ball->vy;

    // ��鲢����x����ı߽���ײ
    if (ball->x <= Boarder->x_min) {
        ball->x = Boarder->x_min;
        ball->vx = -ball->vx;
    } else if (ball->x >= Boarder->x_max) {
        ball->x = Boarder->x_max;
        ball->vx = -ball->vx;
    }

    // ��鲢����y����ı߽���ײ
    if (ball->y <= Boarder->y_min) {
        ball->y = Boarder->y_min;
        ball->vy = -ball->vy;
    } else if (ball->y >= Boarder->y_max) {
        ball->y = Boarder->y_max;
        ball->vy = -ball->vy;
    }
}
