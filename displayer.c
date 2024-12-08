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

#define BRICK_HEIGHT 18
#define BRICK_GAP 1
#define BRICK_TOTAL_HEIGHT (BRICK_HEIGHT + 2 * BRICK_GAP) // ÿש��ռ�õ��ܸ߶�
#define BRICK_WIDTH 8
#define SCREEN_HEIGHT 319
#define BAR_WIDTH 8
#define COLOR_BAR     0xFF0000   // bar color
#define COLOR_BG      0x228B22   //game background color
#define COLOR_BALL    0x8A2BE2   //ball color (138, 43, 226)
#define COLOR_INIT_BG 0x0000FF   //init background gold (255, 215, 0)
//0xFFD700
int Brck_Pos[BRICKS_COUNT][4];
int ball_pixel_counts[MAX_RADIUS + 1][2 * MAX_RADIUS + 1];
int former_x=0;
int former_y=0;
extern int d;
int init_yleft = 100;
int y_bias = 120;
int now_yleft;
int MAX_POS = 199;
int moving_step = 20;
int bullet_velocity = 5;
int score = 0;
Ball ball;
Boarder boarder;


void fillRectColor(uint32_t color, int x1, int y1, int x2, int y2) {
    setColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    fillRect(x1, y1, x2, y2);
}

void drawHLineColor(uint32_t color, int x, int y, int length) {
    setColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    drawHLine(x, y, length);
}

void init_ball_pixel_counts() {
	for (int r = 1; r <= MAX_RADIUS; r++) {
	        for (int dy = -r; dy <= r; dy++) {
	            int dx_limit_squared = r * r - dy * dy;
	            int dx_limit = 0;
	            while (dx_limit * dx_limit <= dx_limit_squared) {
	                dx_limit++;
	            }
	            dx_limit--;
	            ball_pixel_counts[r][dy + r] = 2 * dx_limit + 1;
	        }
	    }
}

void draw_ball(Ball *ball) {
    for (int dy = -ball->radius; dy <= ball->radius; dy++) {
        int dx_count = ball_pixel_counts[ball->radius][dy + ball->radius];
        int x_start = ball->x - (dx_count / 2);
        drawHLineColor(COLOR_BALL, x_start, ball->y + dy, dx_count);
    }
    former_x = ball->x;
    former_y = ball->y;
}

void draw_arrow_init(){
	if(initPlotVar==0){
		fillRectColor(COLOR_INIT_BG, 110, 60, 120, 75);
		DisplText("->"              ,1,140,60,  SmallFont);
	}
	else{
		fillRectColor(COLOR_INIT_BG, 130, 60, 140, 75);
		DisplText("->"              ,1,120,60,  SmallFont);
	}
}
void draw_arrow_settings(){
	fillRectColor(COLOR_INIT_BG, 80, 40, 180, 55);
	if(setChangeFlag==0){
		DisplText("->"              ,1,180,40,  SmallFont);
	}
	else if(setChangeFlag==1){
		DisplText("->"              ,1,160,40,  SmallFont);
	}
	else if(setChangeFlag==2){
		DisplText("->"              ,1,140,40,  SmallFont);
	}
	else if(setChangeFlag==3){
		DisplText("->"              ,1,120,40,  SmallFont);
	}
	else if(setChangeFlag==4){
		DisplText("->"              ,1,100,40,  SmallFont);
	}
}

void dspl_init(){
	fillRectColor(COLOR_INIT_BG, 0, 0, 239, 319);
	DisplText("Welcome to ez game"  , 1,220,0,  BigFont);
	DisplText("Made by:"            , 1,200,4,  SmallFont);
	DisplText("Jiaji & Zhaohongzhi" , 1,180,8,  SmallFont);
	DisplText("START GAME"	        , 1,140,80, SmallFont);
	DisplText("Settings"            , 1,120,80, SmallFont);
	draw_arrow_init();
}
void DisplInt(int a, int rotated,int x,int y,u8* font){
	char buffer[16];
	sprintf(buffer, "%d", a);
	DisplText(buffer,rotated,x,y,font);
}
void dspl_Settings(){
	fillRectColor(COLOR_INIT_BG, 0, 0, 239, 319);
	DisplText("Settings"            , 1,220,0,  BigFont);
	DisplText("Game mode"           , 1,180,60, SmallFont);	 DisplText("Bricks/Board"     , 1,180,200, SmallFont);
	DisplText("Bricks(max 8)"       , 1,160,60, SmallFont);	 DisplInt(valid_sw               , 1,160,200, SmallFont);
	DisplText("Bullet size"         , 1,140,60, SmallFont);  DisplInt(ball.radius            , 1,140,200, SmallFont);
	DisplText("Bullet Velocity"     , 1,120,60, SmallFont);  DisplInt(bullet_velocity        , 1,120,200, SmallFont);
	DisplText("Board Velocity"      , 1,100,60, SmallFont);  DisplInt(moving_step            , 1,100,200, SmallFont);
	draw_arrow_settings();
}
void Game_Init() {
	init_ball_pixel_counts();
	Init_Bricks();
    fillRectColor(COLOR_BG, 0, 0, 239, 319);
    //initBall(&ball, 10, 10, -10, -8, 5);
    set_boarder(&ball, &boarder);
}

void erase_former(int former_x, int former_y, int r) {
    for (int dy = -r; dy <= r; dy++) {
        int dx_count = ball_pixel_counts[r][dy + r];
        int x_start = former_x - (dx_count / 2);
        drawHLineColor(COLOR_BG, x_start, former_y + dy, dx_count);
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
    fillRectColor(COLOR_BG, 0, 0, 8, 319);
    xil_printf("count in Bricks %d\r\n",count);
    if (count > 0) {
        for (int i = 0; i < count; i++) {
            int index = positions[i];
            fillRectColor(COLOR_BAR, Brck_Pos[index][0], Brck_Pos[index][1],
                         Brck_Pos[index][2], Brck_Pos[index][3]);
        }
    }
}

void Score(){

}

void update(){
	erase_former(former_x,former_y,8);
    updateBall(&ball, &boarder);
    draw_ball(&ball);
}


void btn_init_game() {
    now_yleft = init_yleft;
    fillRectColor(COLOR_BAR, 0, now_yleft, BAR_WIDTH, now_yleft + y_bias);
}

void btn_mov_l() {
    if (now_yleft <= moving_step) {
        fillRectColor(COLOR_BG, 0, y_bias + 1, BAR_WIDTH, 2 * y_bias);
        fillRectColor(COLOR_BAR, 0, 0, BAR_WIDTH, y_bias);
        now_yleft = 0;
    } else {
        fillRectColor(COLOR_BG, 0, now_yleft + y_bias - moving_step, BAR_WIDTH, now_yleft + y_bias);
        now_yleft -= moving_step;
        fillRectColor(COLOR_BAR, 0, now_yleft, BAR_WIDTH, now_yleft + moving_step);
    }
}

void btn_mov_r() {
    if (now_yleft + moving_step >= MAX_POS) {
        fillRectColor(COLOR_BG, 0, SCREEN_HEIGHT - 2 * y_bias, BAR_WIDTH, SCREEN_HEIGHT - y_bias);
        fillRectColor(COLOR_BAR, 0, MAX_POS, BAR_WIDTH, SCREEN_HEIGHT);
        now_yleft = MAX_POS;
    } else {
        fillRectColor(COLOR_BG, 0, now_yleft, BAR_WIDTH, now_yleft + moving_step);
        now_yleft += moving_step;
        fillRectColor(COLOR_BAR, 0, now_yleft + y_bias - moving_step, BAR_WIDTH, now_yleft + y_bias);
    }
}

void DisplText(char *s1, int rotated,int x,int y,u8* font) {
    setColor(0, 255, 0);
    setColorBg(0, 0, 255);
    setFont(font);               //SmallFont/BigFont/SevenSegNumFont
    if (rotated) {
        lcdPrintRotated(s1, x, y);
    } else {
        lcdPrint(s1, 75, 140);
    }
}

void dspl_end(){
    //end frame
	DisplText("Game Over"							, 1,140,90,  BigFont);
	DisplText("your score is"						, 1,100,90, SmallFont);
	DisplInt(score									, 1,100,220, SmallFont);
	DisplText("press OK button to start new game"	, 1,80,30, SmallFont);
}
