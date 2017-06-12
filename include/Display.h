/*
 * Display.h
 *
 *  Created on: 2017年5月8日
 *      Author: Romeli
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "cmsis_device.h"

typedef enum _P_Typedef {
	P_CD,
	P_OB,
	P_DF,
	P_AH,
	P_AL,
	P_PC,
	P_BL,
	P_DD,
	P_AD,
	P_OP,
	P_OH,
	P_OL,
	P_ER,
	P_NONE,
	P_LEAD,
	P_Door
} P_Typedef;

class Display {
public:
	static void P(P_Typedef p, uint8_t led = 0x00);
	static void Num(int16_t num, bool isTemp = false, uint8_t led = 0x00);
};

#endif /* DISPLAY_H_ */
