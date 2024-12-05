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

// º¯ÊýÉùÃ÷
void initBall(Ball *ball, int x, int y, int vx, int vy, int radius);
void set_boarder(Ball *ball, Boarder *Boarder);
void updateBall(Ball *ball, Boarder *Boarder);

#endif // LOGIC_H
