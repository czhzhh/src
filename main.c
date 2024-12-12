#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include "qpn_port.h"
#include "bsp.h"
#include "complex.h"
#include "lab2b.h"
#include "lcd.h"
#include "logic.h"
#include "dspl_upd.h"

static QEvent l_lab2bQueue[30];

QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
	{ (QActive *)0,            (QEvent *)0,          0                    },
	{ (QActive *)&l2b,    l_lab2bQueue,         Q_DIM(l_lab2bQueue)  }
};

Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);
volatile int timerTrigger = 0;

void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber)
{
	timerTrigger = 100;
}
int main()
{
	Xil_ICacheInvalidate();
	Xil_ICacheEnable();
	Xil_DCacheInvalidate();
	Xil_DCacheEnable();
	Lab2B_ctor();
	BSP_init(); // inside of bsp.c, starts out empty!
	QF_run();
    return 0;
}
