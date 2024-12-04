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
extern int *positions; // ��̬���������
extern int valid_sw;   // ��ǰ�������󿪹�����
extern int count;
// ��̬��������
void init_positions(int size); // ��ʼ�� positions ����
void free_positions(void);     // �ͷ� positions ����
void BSP_init(void);
void Txt_Set_BLUE(void);
void Tmr_Set_BLUE(void);

#define BSP_showState(prio_, state_) ((void)0)


#endif                                                             /* bsp_h */


