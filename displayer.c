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

#define MAX_RADIUS 20
int ball_pixel_counts[MAX_RADIUS + 1][2 * MAX_RADIUS + 1];
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

void draw_ball(int x, int y, int r) {
	if (r < 1 || r > MAX_RADIUS) return; // 检查半径是否有效

	    for (int dy = -r; dy <= r; dy++) {
	        int dx_count = ball_pixel_counts[r][dy + r]; // 获取预计算的像素点数量
	        int x_start = x - (dx_count / 2); // 计算水平起始点
	        drawHLine(x_start, y + dy, dx_count); // 绘制水平线
	    }
}

void DrawBorder() {
	for (int width = 0; width<6;width++) {
		drawHLine(0, width, DISP_X_SIZE);
		drawHLine(0, DISP_Y_SIZE-width-10, DISP_X_SIZE);
		drawVLine(width, 0, DISP_Y_SIZE);
		drawVLine(DISP_X_SIZE-width, 0, DISP_Y_SIZE);
	}
}

void Set_Blue(){
	for (int Row = 0; Row<8; Row++) {
		int NewRow = Row*40;
		for (int Col = 0; Col<6; Col++) {
			int NewCol = Col*40;
			for (int y = 0; y<40; y++) {
				int blue = 2*ceil(y/2);
				setColor(0,255,255);
				drawHLine(0+NewCol, y+NewRow, 20-(blue/2));
				setColor(0, 0, 255);
				drawHLine(20-(blue/2)+NewCol, y+NewRow, blue);
				setColor(0,255,255);
				drawHLine(20+(blue/2)+NewCol, y+NewRow, 20-(blue/2));
			}
		}
	}
}
void Tmr_Set_Blue(void){
	for (int Row = 1; Row<3; Row++) {
		int NewRow = Row*40;
		for (int Col = 0; Col<6; Col++) {
			int NewCol = Col*40;
			for (int y = 0; y<40; y++) {
				int blue = 2*ceil(y/2);
				setColor(0,255,255);
				drawHLine(0+NewCol, y+NewRow, 20-(blue/2));
				setColor(0, 0, 255);
				drawHLine(20-(blue/2)+NewCol, y+NewRow, blue);
				setColor(0,255,255);
				drawHLine(20+(blue/2)+NewCol, y+NewRow, 20-(blue/2));
			}
		}
	}
}
void Txt_Set_BLUE(void) {
		int NewRow = 3*40;
		for (int Col = 0; Col<6; Col++) {
			int NewCol = Col*40;
			for (int y = 0; y<40; y++) {
				int blue = 2*ceil(y/2);
				setColor(0,255,255);
				drawHLine(0+NewCol, y+NewRow, 20-(blue/2));
				setColor(0, 0, 255);
				drawHLine(20-(blue/2)+NewCol, y+NewRow, blue);
				setColor(0,255,255);
				drawHLine(20+(blue/2)+NewCol, y+NewRow, 20-(blue/2));
		}
	}
}
