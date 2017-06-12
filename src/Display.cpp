/*
 * Display.cpp
 *
 *  Created on: 2017年5月8日
 *      Author: Romeli
 */

#include "Display.h"
#include "TM16xx.h"

typedef enum _DisTab {
	DisTab_h = 16, DisTab_l = 17, DisTab_p = 18, DisTab__ = 19, DisTab_r = 20
} DisTab_Typedef;

const uint8_t DisTab[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f,
		0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x76, 0x38, 0x73, 0x40, 0x50 };

const uint8_t DisTabDot[] = { 0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87,
		0xff, 0xef, 0xf7, 0xfc, 0xb9, 0xde, 0xf9, 0xf1, 0xf6, 0xb8, 0xf3, 0x48,
		0xd0 };

void Display::P(P_Typedef p, uint8_t led) {
	switch (p) {
	case P_CD:
		TM1668.Data[2] = DisTab[DisTab__];
		TM1668.Data[8] = DisTab[DisTab__];
		TM1668.Data[4] = DisTab[12];
		TM1668.Data[6] = DisTab[13];
		break;
	case P_OB:
		TM1668.Data[2] = DisTab[DisTab__];
		TM1668.Data[8] = DisTab[DisTab__];
		TM1668.Data[4] = DisTab[0];
		TM1668.Data[6] = DisTab[11];
		break;
	case P_DF:
		TM1668.Data[2] = DisTab[DisTab__];
		TM1668.Data[8] = DisTab[DisTab__];
		TM1668.Data[4] = DisTab[13];
		TM1668.Data[6] = DisTab[15];
		break;
	case P_AH:
		TM1668.Data[2] = DisTab[DisTab__];
		TM1668.Data[8] = DisTab[DisTab__];
		TM1668.Data[4] = DisTab[10];
		TM1668.Data[6] = DisTab[DisTab_h];
		break;
	case P_AL:
		TM1668.Data[2] = DisTab[DisTab__];
		TM1668.Data[8] = DisTab[DisTab__];
		TM1668.Data[4] = DisTab[10];
		TM1668.Data[6] = DisTab[DisTab_l];
		break;
	case P_PC:
		TM1668.Data[2] = DisTab[DisTab__];
		TM1668.Data[8] = DisTab[DisTab__];
		TM1668.Data[4] = DisTab[DisTab_p];
		TM1668.Data[6] = DisTab[12];
		break;
	case P_BL:
		TM1668.Data[2] = DisTab[DisTab__];
		TM1668.Data[8] = DisTab[DisTab__];
		TM1668.Data[4] = DisTab[11];
		TM1668.Data[6] = DisTab[DisTab_l];
		break;
	case P_DD:
		TM1668.Data[2] = DisTab[DisTab__];
		TM1668.Data[8] = DisTab[DisTab__];
		TM1668.Data[4] = DisTab[13];
		TM1668.Data[6] = DisTab[13];
		break;
	case P_AD:
		TM1668.Data[2] = DisTab[DisTab__];
		TM1668.Data[8] = DisTab[DisTab__];
		TM1668.Data[4] = DisTab[10];
		TM1668.Data[6] = DisTab[13];
		break;
	case P_OP:
		TM1668.Data[0] = led;
		TM1668.Data[2] = DisTab[DisTab__];
		TM1668.Data[8] = DisTab[DisTab__];
		TM1668.Data[4] = DisTab[0];
		TM1668.Data[6] = DisTab[DisTab_p];
		break;
	case P_OH:
		TM1668.Data[0] = led;
		TM1668.Data[2] = DisTab[DisTab__];
		TM1668.Data[8] = DisTab[DisTab__];
		TM1668.Data[4] = DisTab[0];
		TM1668.Data[6] = DisTab[DisTab_h];
		break;
	case P_OL:
		TM1668.Data[0] = led;
		TM1668.Data[2] = DisTab[DisTab__];
		TM1668.Data[8] = DisTab[DisTab__];
		TM1668.Data[4] = DisTab[0];
		TM1668.Data[6] = DisTab[DisTab_l];
		break;
	case P_ER:
		TM1668.Data[0] = led;
		TM1668.Data[2] = DisTab[DisTab__];
		TM1668.Data[8] = DisTab[DisTab__];
		TM1668.Data[4] = DisTab[14];
		TM1668.Data[6] = DisTab[DisTab_r];
		break;
	case P_NONE:
		TM1668.Data[2] = 0;
		TM1668.Data[4] = 0;
		TM1668.Data[6] = 0;
		TM1668.Data[8] = 0;
		break;
	case P_LEAD:
		TM1668.Data[0] = led;
		TM1668.Data[2] = DisTab[DisTab_l];
		TM1668.Data[4] = DisTab[14];
		TM1668.Data[6] = DisTab[10];
		TM1668.Data[8] = DisTab[13];
		break;
	case P_Door: {
		TM1668.Data[0] = led;
		TM1668.Data[2] = DisTab[13];
		TM1668.Data[4] = DisTab[0];
		TM1668.Data[6] = DisTab[0];
		TM1668.Data[8] = DisTab[DisTab_r];
		break;
	}
	default:
		TM1668.Data[2] = 0;
		TM1668.Data[4] = 0;
		TM1668.Data[6] = 0;
		TM1668.Data[8] = 0;
		break;
	}
}

void Display::Num(int16_t num, bool isTemp, uint8_t led) {
	if (isTemp) {
		TM1668.Data[0] = led;

		if (num >= 0) {
			if (num >= 1000) {
				TM1668.Data[2] = DisTab[num / 1000];
			} else {
				TM1668.Data[2] = 0x00;
			}
		} else {
			TM1668.Data[2] = DisTab[DisTab__];
			num = -num;
		}

		if (num >= 100) {
			TM1668.Data[4] = DisTab[num % 1000 / 100];
		} else {
			TM1668.Data[4] = 0x00;
		}

		TM1668.Data[6] = DisTabDot[num % 100 / 10];

		TM1668.Data[8] = DisTab[num % 10];
	} else {
		if (num >= 1000) {
			TM1668.Data[2] = DisTab[num / 1000];
		} else {
			TM1668.Data[2] = 0x00;
		}

		if (num >= 100) {
			TM1668.Data[4] = DisTab[num % 1000 / 100];
		} else {
			TM1668.Data[4] = 0x00;
		}

		if (num >= 10) {
			TM1668.Data[6] = DisTab[num % 100 / 10];
		} else {
			TM1668.Data[6] = 0x00;
		}

		TM1668.Data[8] = DisTab[num % 10];
	}
}
