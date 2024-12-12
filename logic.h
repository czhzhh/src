#ifndef LOGIC_H
#define LOGIC_H

#define MAX_RADIUS 20
#define MAX_VELOCITIES 5

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

typedef struct {
    int vx[MAX_VELOCITIES];
    int vy[MAX_VELOCITIES];
    int current_index;  // ��ǰʹ�õ�����
} VelocityArray;


void initBall(Ball *ball, int x, int y, int vx, int vy, int radius);
void set_boarder(Ball *ball, Boarder *Boarder);
void updateBall(Ball *ball, Boarder *Boarder);
void upbtn_setting_change(int* currentMode,Ball *ball, int* moving_step,int *bullet_velocity, int *y_bias);
void downbtn_setting_change(int* currentMode,Ball *ball, int* moving_step,int *bullet_velocity, int *y_bias);
int calculateReflect(int currentPositionY, int now_yleft, int y_bias, int Brck_Pos[][4], int BRICKS_COUNT, int currentMode, int score, int ball_radius);
void generateSpeed(int *vx, int *vy);
void init_vel(int initial_v);
void updateBallSpeed(Ball* ball);
void init_velocities(int initial_v);
#endif // LOGIC_H
