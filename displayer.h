#ifndef displayer_h
#define displayer_h

//int former_x=0;
//int former_y=0;

void Init_game();
void init_ball_pixel_counts();
void draw_ball(int x, int y, int r);
void DrawBorder();
void Set_Blue();
void Tmr_Set_Blue(void);
void Txt_Set_BLUE(void);
void dspl_init();
void dspl_Settings();
void erase_former();
void Init_Bricks(void);
void Bricks(int count, int *positions);
void update();
#endif
