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
#include "displayer.h"
#include "lcd.h"
#include "logic.h"


#define MAX_RADIUS 20
#define BRICKS_COUNT 16
#define BRICK_HEIGHT 18
#define BRICK_GAP 1
#define BRICK_TOTAL_HEIGHT (BRICK_HEIGHT + 2 * BRICK_GAP) // 每砖块占用的总高度
#define BRICK_WIDTH 8

int Brck_Pos[BRICKS_COUNT][4];
int ball_pixel_counts[MAX_RADIUS + 1][2 * MAX_RADIUS + 1];
int former_x=0;
int former_y=0;
extern int d;

Ball ball;
Boarder boarder;
void Init_game(){
	init_ball_pixel_counts();
	Init_Bricks();
}
void init_ball_pixel_counts() {
	for (int r = 1; r <= MAX_RADIUS; r++) {
	        for (int dy = -r; dy <= r; dy++) {
	            int dx_limit_squared = r * r - dy * dy;
	            int dx_limit = 0;

	            // 计算水平范围，避免浮点运算
	            while (dx_limit * dx_limit <= dx_limit_squared) {
	                dx_limit++;
	            }
	            dx_limit--; // 回退一步，找到最大的有效整数 dx_limit

	            // 存储像素点数量（每行总长度）
	            ball_pixel_counts[r][dy + r] = 2 * dx_limit + 1;
	        }
	    }
}

void draw_ball(Ball *ball) {
	setColor(138, 43, 226);
	    for (int dy = -ball->radius; dy <= ball->radius; dy++) {
	        int dx_count = ball_pixel_counts[ball->radius][dy + ball->radius]; // 获取预计算的像素点数量
	        int x_start = ball->x - (dx_count / 2); // 计算水平起始点
	        drawHLine(x_start, ball->y + dy, dx_count); // 绘制水平线
	    }
	    former_x=ball->x;
	    former_y=ball->y;
}

//void DrawBorder() {
//	for (int width = 0; width<6;width++) {
//		drawHLine(0, width, DISP_X_SIZE);
//		drawHLine(0, DISP_Y_SIZE-width-10, DISP_X_SIZE);
//		drawVLine(width, 0, DISP_Y_SIZE);
//		drawVLine(DISP_X_SIZE-width, 0, DISP_Y_SIZE);
//	}
//}
//void Set_Blue(){
//	for (int Row = 0; Row<8; Row++) {
//		int NewRow = Row*40;
//		for (int Col = 0; Col<6; Col++) {
//			int NewCol = Col*40;
//			for (int y = 0; y<40; y++) {
//				int blue = 2*ceil(y/2);
//				setColor(0,255,255);
//				drawHLine(0+NewCol, y+NewRow, 20-(blue/2));
//				setColor(0, 0, 255);
//				drawHLine(20-(blue/2)+NewCol, y+NewRow, blue);
//				setColor(0,255,255);
//				drawHLine(20+(blue/2)+NewCol, y+NewRow, 20-(blue/2));
//			}
//		}
//	}
//}
//void Tmr_Set_Blue(void){
//	for (int Row = 1; Row<3; Row++) {
//		int NewRow = Row*40;
//		for (int Col = 0; Col<6; Col++) {
//			int NewCol = Col*40;
//			for (int y = 0; y<40; y++) {
//				int blue = 2*ceil(y/2);
//				setColor(0,255,255);
//				drawHLine(0+NewCol, y+NewRow, 20-(blue/2));
//				setColor(0, 0, 255);
//				drawHLine(20-(blue/2)+NewCol, y+NewRow, blue);
//				setColor(0,255,255);
//				drawHLine(20+(blue/2)+NewCol, y+NewRow, 20-(blue/2));
//			}
//		}
//	}
//}
//void Txt_Set_BLUE(void) {
//		int NewRow = 3*40;
//		for (int Col = 0; Col<6; Col++) {
//			int NewCol = Col*40;
//			for (int y = 0; y<40; y++) {
//				int blue = 2*ceil(y/2);
//				setColor(0,255,255);
//				drawHLine(0+NewCol, y+NewRow, 20-(blue/2));
//				setColor(0, 0, 255);
//				drawHLine(20-(blue/2)+NewCol, y+NewRow, blue);
//				setColor(0,255,255);
//				drawHLine(20+(blue/2)+NewCol, y+NewRow, 20-(blue/2));
//		}
//	}
//}
void dspl_init(){
	/*
	 * Game start
	 * Settings
	 *
	 * */
}
void dspl_Settings(){
	/*
	 * Difficulty(Velocity)
	 * Bullet Size
	 * Number of Bouncers
	 *
	 * */
}
void dspl_game_Init(){
	setColor(34, 139, 34);
	fillRect(0,0,239,319);
	initBall(&ball,10,10,10,10,5);
	set_boarder(&ball,&boarder);
}
void erase_former(int former_x,int former_y,int r){
	/*SHOULD USE ERASER BEFORE UPDATE
	 * erase last ball
	 * erase last score
	 * */
	setColor(34, 139, 34);
	for (int dy = -r; dy <= r; dy++) {
	    int dx_count = ball_pixel_counts[r][dy + r]; // 获取预计算的像素点数量
	    int x_start = former_x - (dx_count / 2); // 计算水平起始点
	    drawHLine(x_start, former_y + dy, dx_count); // 绘制水平线
	}
}
void Init_Bricks(void) {
    int x1 = 1;
    int x2 = x1 + BRICK_WIDTH - 1;

    for (int i = 0; i < BRICKS_COUNT; i++) {
        int y1 = i * BRICK_TOTAL_HEIGHT + BRICK_GAP;
        int y2 = y1 + BRICK_HEIGHT - 1;

        Brck_Pos[i][0] = x1;
        Brck_Pos[i][1] = y1;
        Brck_Pos[i][2] = x2;
        Brck_Pos[i][3] = y2;
    }
}
void Bricks(int count, int *positions) {
    setColor(34, 139, 34);
    fillRect(0, 0, 8, 319);
    if (count > 0) {
        setColor(255, 0, 0);
        for (int i = 0; i < count; i++) {
            int index = positions[i];
            int x1 = Brck_Pos[index][0];
            int y1 = Brck_Pos[index][1];
            int x2 = Brck_Pos[index][2];
            int y2 = Brck_Pos[index][3];
            fillRect(x1, y1, x2, y2);
        }
    }
}
void Score(){

}

void update(){
	erase_former(former_x,former_y,8);
	printf("Before updateBall: x=%d, y=%d, address=%p\n", ball.x, ball.y, &ball);
    updateBall(&ball, &boarder);
    printf("After updateBall: x=%d, y=%d, address=%p\n", ball.x, ball.y, &ball);
    draw_ball(&ball);
    printf("After draw_ball: x=%d, y=%d, address=%p\n", ball.x, ball.y, &ball);
}





















