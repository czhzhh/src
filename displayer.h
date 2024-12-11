#ifndef displayer_h
#define displayer_h


#include "logic.h"

void Init_game();
void init_ball_pixel_counts();
void draw_ball(Ball *ball);
void DrawBorder();
void Set_Blue();
void Tmr_Set_Blue(void);
void Txt_Set_BLUE(void);
void dspl_init();
void dspl_Settings();
void erase_former(int former_x,int former_y,int r);
void Init_Bricks(void);
void Bricks(int count, int *positions);
void update();
void btn_init_game();
void btn_mov_l();
void btn_mov_r();
void fillRectColor(uint32_t color, int x1, int y1, int x2, int y2);
void DisplText(char *s1, int rotated,int x,int y,u8* font);
void Game_Init();
void dspl_end();
void draw_arrow_init();
void draw_arrow_settings();
void DisplInt(int a, int rotated,int x,int y,u8* font);
void dspl_Br_Bo();

#define BRICKS_COUNT 16

extern int moving_step;
extern int bullet_velocity;
extern Ball ball;
extern int Brck_Pos[BRICKS_COUNT][4];
extern int y_bias;
extern int now_yleft;
extern int init_yleft;
extern int score;
extern Boarder boarder;
extern int y_bias;
#endif
