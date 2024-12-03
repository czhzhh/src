#ifndef lab2b_h
#define lab2b_h


extern int act_volume;
extern int MainVolumeState;
extern int VolumeState;
extern int MainTextState;
extern int TextState;

enum Lab2BSignals {
	ENCODER_UP = Q_USER_SIG,
	ENCODER_DOWN,
	ENC_PRS,
	B_U,
	B_D,
	B_L,
	B_R,
	B_C
};


extern struct Lab2BTag l2b;

void Lab2B_ctor(void);
void GpioHandler(void *CallbackRef);
void TwistHandler(void *CallbackRef);
void SWHandler(void *CallbackRef);
void Tmr_Cter_Hdler(void *CallbackRef);
void draw_ball(int x, int y, int r);

#endif  
