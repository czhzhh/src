/*****************************************************************************
* lab2B.c for Lab2B of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/

#include "qpn_port.h"
#include "bsp.h"
#include "lab2b.h"
#include "lcd.h"
#include "displayer.h"
#include <math.h>




typedef struct Lab2BTag  {               //Lab2B State machine
	QActive super;
} Lab2B;
int act_volume = 0;
int stored_value = 0;
int Mute = 0;
int MainVolumeState = 0;
int VolumeState = 0;
int MainTextState = 0;
int TextState = 0;
int last_vol=0;
volatile int d=0;
/* Setup state machines */
/**********************************************************************/
static QState Lab2B_initial (Lab2B *me);
static QState Lab2B_on      (Lab2B *me);
static QState Lab2B_A  (Lab2B *me);
static QState Lab2B_B  (Lab2B *me);


/**********************************************************************/
Lab2B l2b;


void Lab2B_ctor(void)  {
	Lab2B *me = &l2b;
	QActive_ctor(&me->super, (QStateHandler)&Lab2B_initial);
}


QState Lab2B_initial(Lab2B *me) {
	xil_printf("\n\rInitialization");
    return Q_TRAN(&Lab2B_on);
}

QState Lab2B_on(Lab2B *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			xil_printf("On\n");
			}
		case Q_INIT_SIG: {
			return Q_TRAN(&Lab2B_A);
			}
	}
	return Q_SUPER(&QHsm_top);
}
void Vol_up(){
	printf("Volume: %d\n",act_volume);
	setColor(0, 0, 0);
	fillRect(act_volume+70, 90, 170, 110);
	setColorBg(255, 0, 0);
	fillRect(act_volume+69, 90, act_volume+70, 110);
	MainVolumeState=1;
	VolumeState=1;
}
void Vol_down(){
	printf("Volume: %d\n",act_volume);
	setColor(0, 0, 0);
	fillRect(act_volume+71, 90, 170, 110);
	setColor(0, 0, 0);
	fillRect(act_volume+70, 90, act_volume+71, 110);
	MainVolumeState = 1;
	VolumeState = 1;
}

void VolumeToggle(int toggle) {
	if (toggle == 1) {
		last_vol=act_volume;
		act_volume=0;
		setColor(0, 0, 0);
		fillRect(70, 90, 170, 110);
		MainVolumeState = 1;
		VolumeState = 1;
	} 
	else {
		act_volume=last_vol;
		setColor(0, 255, 0);
		fillRect(69, 90, 70+act_volume, 110);
		setColor(0, 0, 0);
		fillRect(act_volume+71, 90, 170, 110);
		MainVolumeState=1;
		VolumeState=1;
	}
}
void DisplText(char* s1) {
	Txt_Set_BLUE(); 
	setColor(255, 191, 50);
	setColorBg(255, 255, 255);
	setFont(BigFont);
	lcdPrint(s1, 75, 140);
	MainTextState = 1;
	TextState = 1;
}
/* Create Lab2B_on state and do any initialization code if needed */
/******************************************************************/
QState Lab2B_A(Lab2B *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			xil_printf("Startup State A\n");
			return Q_HANDLED();
		}
		case ENCODER_UP: {
			xil_printf("Encoder Up in State A\n");
			if(act_volume<100){
				act_volume++;
			}
			else if (act_volume >= 100) {		//Worst case if bugged
							act_volume = 100;
						}
			Vol_up();
			return Q_HANDLED();
		}
		case ENCODER_DOWN: {
			xil_printf("Encoder Down in State A\n");
			if(act_volume>0)act_volume--;
			else if (act_volume <= 0) {		//Worst case if bugged
							act_volume = 0;
						}
			Vol_down();
			return Q_HANDLED();
		}
		case ENC_PRS:{
			xil_printf("Changing State\n");
			Mute=1;
			VolumeToggle(Mute);
			return Q_TRAN(&Lab2B_B);
		}
		case B_U:{
			xil_printf("upper button pressed\n");
			update();
			d+=1;
			return Q_HANDLED();
		}
		case B_D:{
			xil_printf("down button pressed\n");
			dspl_game_Init();
			return Q_HANDLED();
		}
		case B_L:{
			xil_printf("left button pressed\n");
			DisplText("have");
			return Q_HANDLED();
		}
		case B_R:{
			xil_printf("right button pressed\n");
			DisplText("no");
			return Q_HANDLED();
		}
		case B_C:{
			xil_printf("centurl button pressed\n");
			DisplText("parties");
			return Q_HANDLED();
		}
	}
	return Q_SUPER(&Lab2B_on);
}

QState Lab2B_B(Lab2B *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			xil_printf("Startup State B\n");
			return Q_HANDLED();
		}
		
		case ENCODER_UP: {
			xil_printf("Encoder Up in State B\n");
			Mute=0;
			act_volume=0;
			Vol_up();
			return Q_TRAN(&Lab2B_A);
		}

		case ENCODER_DOWN: {
			xil_printf("Encoder Down from State B\n");
			return Q_HANDLED();
		}

		case ENC_PRS:  {
			xil_printf("Changing State\n");
			Mute=0;
			VolumeToggle(Mute);
			return Q_TRAN(&Lab2B_A);
		}
		case B_U:{
			xil_printf("upper button pressed\n");
			DisplText("ECE");
			return Q_HANDLED();
		}
		case B_D:{
			xil_printf("down button pressed\n");
			DisplText("guys");
			return Q_HANDLED();
		}
		case B_L:{
			xil_printf("left button pressed\n");
			DisplText("have");
			return Q_HANDLED();
		}
		case B_R:{
			xil_printf("right button pressed\n");
			DisplText("no");
			return Q_HANDLED();
		}
		case B_C:{
			xil_printf("centurl button pressed\n");
			DisplText("parties");
			return Q_HANDLED();
		}
	}
	return Q_SUPER(&Lab2B_on);
}

