/*****************************************************************************
* bsp.h for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/
#ifndef bsp_h
#define bsp_h


                                              

extern int act_volume;
extern int MainVolumeState;
extern int VolumeState;
extern int MainTextState;
extern int TextState;
extern int *positions; // 动态分配的数组
extern int valid_sw;   // 当前允许的最大开关数量
extern int count;
// 动态调整函数
void init_positions(int size); // 初始化 positions 数组
void free_positions(void);     // 释放 positions 数组
void BSP_init(void);
void Txt_Set_BLUE(void);
void Tmr_Set_BLUE(void);

#define BSP_showState(prio_, state_) ((void)0)


#endif                                                             /* bsp_h */


