#ifndef LOGIC_H
#define LOGIC_H

#define MAX_RADIUS 20

typedef struct {
    int x;
    int y;
    int vx;
    int vy;
    int radius;
} Ball;

typedef struct {
    int x_min;
    int x_max;
    int y_min;
    int y_max;
} Boarder;

// ��������
void initBall(Ball *ball, int x, int y, int vx, int vy, int radius);
void set_boarder(Ball *ball, Boarder *Boarder);
void updateBall(Ball *ball, Boarder *Boarder);
void upbtn_setting_change(Ball *ball, int moving_step);
void downbtn_setting_change(Ball *ball, int moving_step);
int calculateReflect(int currentPositionY, int now_yleft, int y_bias, int Brck_Pos[][4], int BRICKS_COUNT, int currentMode, int score);
void regenerateSpeed(int *vx, int *vy);
#endif // LOGIC_H
