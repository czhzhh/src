/*****************************************************************************
* bsp.c for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 27,2019
*****************************************************************************/

/**/
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




/*****************************/


/* Define all variables and Gpio objects here  */
//interrupt settings
static XIntc sys_intc;
static XGpio enc_gpio;
static XTmrCtr axiTimer;
static XGpio dc;
static XSpi spi;
static XGpio btn;
static XGpio sw;

#define RESET_VALUE 0x5F5E100
#define GPIO_CHANNEL1 1
#define BUTTON_CHANNEL 1
#define SW_CHANNEL 1

unsigned int toggle = 1;
static int encoder_count = 4;
volatile int count = 0;
volatile int timer_state=0;
int VolumeTimeOut = 0;
int TextTimeOut = 0;
static Xuint16 state = 0b11;
int valid_sw = 5;

static enum STATES {
		S0 = 0b11,
		S1 = 0b01,
		S2 = 0b00,
		S3 = 0b10
};
#define MAX_RADIUS 20

// 用于存储不同半径的球每行的像素点数量
int ball_pixel_counts[MAX_RADIUS + 1][2 * MAX_RADIUS + 1];

// 初始化数组，计算每行的像素点数量
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
void BSP_init(void) {
	init_ball_pixel_counts();
    XSpi_Config *spiConfig;  /* Pointer to Configuration data */
    u32 controlReg;

    /* ----- Initialize Interrupt Controller ----- */
    XIntc_Initialize(&sys_intc, XPAR_INTC_0_DEVICE_ID);

    /* ----- Initialize ENCODER GPIO ----- */
    XGpio_Initialize(&enc_gpio, XPAR_ENCODER_DEVICE_ID);
    XIntc_Connect(&sys_intc, XPAR_INTC_0_GPIO_0_VEC_ID, (Xil_ExceptionHandler)TwistHandler, &enc_gpio);
    XIntc_Enable(&sys_intc, XPAR_INTC_0_GPIO_0_VEC_ID);
    XGpio_InterruptEnable(&enc_gpio, GPIO_CHANNEL1);
    XGpio_InterruptGlobalEnable(&enc_gpio);

    /* ----- Initialize SW GPIO ----- */
    XGpio_Initialize(&sw, XPAR_SWITCH_DEVICE_ID);
    XIntc_Connect(&sys_intc, XPAR_INTC_0_GPIO_5_VEC_ID, (Xil_ExceptionHandler)SWHandler, &sw);
    XIntc_Enable(&sys_intc, XPAR_INTC_0_GPIO_5_VEC_ID);
    XGpio_InterruptEnable(&sw, SW_CHANNEL);
    XGpio_InterruptGlobalEnable(&sw);

    /* ----- Initialize Button GPIO ----- */
    XGpio_Initialize(&btn, XPAR_AXI_GPIO_BTN_DEVICE_ID);
    XIntc_Connect(&sys_intc, XPAR_INTC_0_GPIO_1_VEC_ID, (Xil_ExceptionHandler)GpioHandler, &btn);
    XIntc_Enable(&sys_intc, XPAR_INTC_0_GPIO_1_VEC_ID);
    XGpio_InterruptEnable(&btn, BUTTON_CHANNEL);
    XGpio_InterruptGlobalEnable(&btn);

    /* ----- Initialize Timer ----- */
    XTmrCtr_Initialize(&axiTimer, XPAR_AXI_TIMER_0_DEVICE_ID);
    XIntc_Connect(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR, (XInterruptHandler)XTmrCtr_InterruptHandler, (void *)&axiTimer);
    XIntc_Enable(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);

    XTmrCtr_SetHandler(&axiTimer, Tmr_Cter_Hdler, &axiTimer);
    XTmrCtr_SetOptions(&axiTimer, 0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
    XTmrCtr_SetResetValue(&axiTimer, 0, 0xFFFF0000);
    XTmrCtr_Start(&axiTimer, 0);
    xil_printf("Timer start!\n");

    /* ----- Start Interrupt Controller and Enable Global Interrupts ----- */
    XIntc_Start(&sys_intc, XIN_REAL_MODE);
    microblaze_enable_interrupts();

    /* ----- Initialize SPI ----- */
    XGpio_Initialize(&dc, XPAR_SPI_DC_DEVICE_ID);
    XGpio_SetDataDirection(&dc, 1, 0x0);
    spiConfig = XSpi_LookupConfig(XPAR_SPI_DEVICE_ID);
    XSpi_CfgInitialize(&spi, spiConfig, spiConfig->BaseAddress);
    XSpi_Reset(&spi);
    controlReg = XSpi_GetControlReg(&spi);
    XSpi_SetControlReg(&spi, (controlReg | XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK) & (~XSP_CR_TRANS_INHIBIT_MASK));
    XSpi_SetSlaveSelectReg(&spi, ~0x01);

    /* ----- Register Interrupt Handler for MICROBLAZE ----- */
    microblaze_register_handler((XInterruptHandler)XIntc_DeviceInterruptHandler, (void *)XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);
    microblaze_enable_interrupts();
}

/*..........................................................................*/
void QF_onStartup(void) {                 /* entered with interrupts locked */

/* Enable interrupts */
	xil_printf("QF_onStartup\n"); // Comment out once you are in your complete program
	initLCD();
	clrScr();
	Set_Blue();
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
void QF_onIdle(void) {QF_INT_UNLOCK();}

/* Q_onAssert is called only when the program encounters an error*/
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* name of the file that throws the error */
    (void)line;                                   /* line of the code that throws the error */
    QF_INT_LOCK();
    for (;;) {
    }
}

int analyzeBits(uint32_t value, int valid_sw, int *positions) {
    int count = 0; // 统计被置1的位数
    for (int i = 15; i >= 0 && count <= valid_sw; i--) {
        if (value & (1 << i)) { // 如果第 i 位为 1
            if (count < valid_sw) {
                positions[count] = i + 1; // 记录位置，从 1 开始计数
            }
            count++;
        }
    }
    return (count > valid_sw) ? 0 : count;
}

void SWHandler(void *CallbackRef) {
    // Increment A counter
    XGpio *GpioPtr = (XGpio *)CallbackRef;
    XGpio_InterruptClear(GpioPtr, SW_CHANNEL); // 清除中断
    Xuint32 ButtonPressStatus = 0;
    ButtonPressStatus = XGpio_DiscreteRead(&sw, SW_CHANNEL);
    int positions[valid_sw];
    int count = analyzeBits(ButtonPressStatus, valid_sw, positions);
    if (count == 0) {
            xil_printf("Invalid\n\r");
        } else {
            xil_printf("Valid: %d positions: ", count);
            for (int i = 0; i < count; i++) {
                xil_printf("%d ", positions[i]);
            }
            xil_printf("\n\r");
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

void GpioHandler(void *CallbackRef) {
	// Increment A counter
	XGpio *GpioPtr = (XGpio *)CallbackRef;
	XGpio_InterruptClear(GpioPtr, BUTTON_CHANNEL);	// Clearing interrupt
	Xuint32 ButtonPressStatus = 0;
	ButtonPressStatus = XGpio_DiscreteRead(&btn, BUTTON_CHANNEL);
	if (ButtonPressStatus == 0x04) {
			QActive_postISR((QActive *)&l2b, B_R);
		}
		else if (ButtonPressStatus == 0x02) {
			QActive_postISR((QActive *)&l2b, B_L);
		}
		else if (ButtonPressStatus == 0x10) {
			QActive_postISR((QActive *)&l2b, B_C);
		}
		else if (ButtonPressStatus == 0x01) {
			QActive_postISR((QActive *)&l2b, B_U);
		}
		else if (ButtonPressStatus == 0x08) {
			QActive_postISR((QActive *)&l2b, B_D);
		}
}

void TwistHandler(void *CallbackRef) {
	Xuint32 encoderStatus = 0;
	XGpio *GpioPtr = (XGpio *)CallbackRef;
	Xuint32 start = XTmrCtr_GetTimerCounterReg(axiTimer.BaseAddress, 0);
	Xuint32 finish = start;

	while (finish < (start + RESET_VALUE/10000)) {
		finish = XTmrCtr_GetTimerCounterReg(axiTimer.BaseAddress, 0);
	}
	encoderStatus = XGpio_DiscreteRead(&enc_gpio, 1);
	if (encoderStatus == 7) {
		for(int i=0;i<=500000;i++);
			toggle = !toggle;
			state = S0;
			encoder_count = 4;
			QActive_postISR((QActive *)&l2b,ENC_PRS);
		}
		switch (state) {
			case S0: {
				{if (encoder_count == 8 || encoder_count == 0) {
					encoder_count = 4;
				}
				}
				switch(encoderStatus) {
					case 0b01: {
						if(encoder_count == 4) {
							encoder_count = encoder_count + 1;
							state = S1;
						}
						break;}
					case 0b10: {
						if(encoder_count == 4) {
							encoder_count = encoder_count - 1;
							state = S3;
						}
						break;}
					break;}
				break;}
			case S1: {
				switch(encoderStatus) {
				case 0b11: {if(encoder_count == 1) {
					encoder_count = encoder_count - 1;
					state = S0;
					QActive_postISR((QActive *)&l2b,ENCODER_DOWN);}
				break;}
				case 0b00: {
					if(encoder_count == 5) {
						encoder_count = encoder_count + 1;
						state = S2;
						}
					break;}
				break;}
				break;}
			case S2: {switch(encoderStatus) {
					case 0b01:{
						if (encoder_count == 2) {
							encoder_count = encoder_count - 1;
							state = S1;}
						break;}
					case 0b10:
					{if (encoder_count == 6) {
						encoder_count = encoder_count + 1;
						state = S3;}
					break;}
					break;}
			break;}
			case S3: {switch(encoderStatus) {
					case 0b00:
					{if(encoder_count == 3) {
						encoder_count = encoder_count - 1;
						state = S2;}
					break;}
					case 0b11:
					{if(encoder_count == 7) {
						encoder_count = encoder_count + 1;
						state = S0;
						QActive_postISR((QActive *)&l2b,ENCODER_UP);}
					break;}
					break;}
			break;}
			break;}
	XGpio_InterruptClear(GpioPtr, GPIO_CHANNEL1);
}
void Tmr_Cter_Hdler(void *CallbackRef){
	if(MainVolumeState==1){
		if (VolumeState == 1) {
		setColor(0,255,00);
		fillRect(70, 90, act_volume+70, 110);
		VolumeTimeOut = 0;
		VolumeState = 0;
	}
	if(VolumeTimeOut>3069){
		printf("time out vol\n");
		Tmr_Set_Blue();
		MainVolumeState=0;
	}
		VolumeTimeOut++;}

	if(MainTextState==1){
		if(TextState==1){
			TextTimeOut=0;
			TextState=0;
		}
		if(TextTimeOut>3069){
			printf("time out txt\n");
			Txt_Set_BLUE();
			MainTextState=0;
		}
		TextTimeOut++;
	}
}
