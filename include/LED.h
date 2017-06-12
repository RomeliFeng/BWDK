/*
 * LED.h
 *
 *  Created on: 2017年5月10日
 *      Author: Romeli
 */

#ifndef LED_H_
#define LED_H_

#include "cmsis_device.h"

typedef enum _LED_Typedef {
	LED_Max = 0x08, LED_Round = 0x40, LED_Hand = 0x10
} LED_Typedef;

class LED {
public:
	static void Control(LED_Typedef led, Status_Typedef status);
	static Status_Typedef Status(LED_Typedef led);
};

#endif /* LED_H_ */
