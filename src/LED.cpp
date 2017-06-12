/*
 * LED.cpp
 *
 *  Created on: 2017年5月10日
 *      Author: Romeli
 */

#include "LED.h"
#include "TM16xx.h"

void LED::Control(LED_Typedef led, Status_Typedef status) {
	//设置模式LED
	if (status == Status_On) {
		TM1668.Data[10] |= led;
	} else {
		TM1668.Data[10] &= ~led;
	}
}

Status_Typedef LED::Status(LED_Typedef led) {
	//判断模式LED状态
	if ((TM1668.Data[10] & led) != 0) {
		return Status_On;
	} else {
		return Status_Off;
	}
}
