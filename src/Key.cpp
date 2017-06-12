/*
 * Key.cpp
 *
 *  Created on: 2017年5月10日
 *      Author: Romeli
 */

#include "Key.h"
#include "TM16xx.h"
#include "Delay.h"

KeyStatus_Typedef Key::Check(Key_Typedef key, uint16_t wait) {
	uint32_t last = millis();
	switch (key) {
	case Key_SET:
		if (TM1668.Key[0].bit.bit0 != 0) {
			//等待按键结束
			while (TM1668.Key[0].bit.bit0) {
				if (wait != 0 && (millis() - last > wait)) {
					//超过等待时间，返回为长按
					return KeyStatus_LoogPress;
				}
			}
			Delay_ms(50);
			return KeyStatus_Press;
		}
		break;
	case Key_UP:
		if (TM1668.Key[0].bit.bit3 != 0) {
			//等待按键结束
			while (TM1668.Key[0].bit.bit3) {
				if (wait != 0 && (millis() - last > wait)) {
					//超过等待时间，返回为长按
					return KeyStatus_LoogPress;
				}
			}
			Delay_ms(50);
			return KeyStatus_Press;
		}
		break;
	case Key_DOWN:
		if (TM1668.Key[1].bit.bit0 != 0) {
			//等待按键结束
			while (TM1668.Key[1].bit.bit0) {
				if (wait != 0 && (millis() - last > wait)) {
					//超过等待时间，返回为长按
					return KeyStatus_LoogPress;
				}
			}
			Delay_ms(50);
			return KeyStatus_Press;
		}
		break;
	case Key_SILENCE:
		if (TM1668.Key[1].bit.bit3 != 0) {
			//等待按键结束
			while (TM1668.Key[1].bit.bit3) {
				if (wait != 0 && (millis() - last > wait)) {
					//超过等待时间，返回为长按
					return KeyStatus_LoogPress;
				}
			}
			Delay_ms(50);
			return KeyStatus_Press;
		}
		break;
	case Key_ESC:
		if (TM1668.Key[2].bit.bit0 != 0) {
			//等待按键结束
			while (TM1668.Key[2].bit.bit0) {
				if (wait != 0 && (millis() - last > wait)) {
					return KeyStatus_LoogPress;
				}
			}
			Delay_ms(50);
			return KeyStatus_Press;
		}
		break;
	default:
		break;
	}
	return KeyStatus_None;
}

void Key::Wait(Key_Typedef key, uint16_t wait) {
	uint32_t last = millis();
	while (true) {
		if ((millis() - last > wait) || (Check(key) != KeyStatus_None)) {
			return;
		}
	}
}

void Key::Input(P_Typedef p, int16_t* num, int16_t lower, int16_t upper,
		bool isTemp, uint8_t led) {
	uint32_t last = millis();
	uint16_t wait = 100;
	KeyStatus_Typedef status;
	uint8_t loogPressCount[2] = { 0, 0 };

	Display::P(p);
	Key::Wait(Key_SET);

	Display::Num(*num, isTemp, led);
	while (true) {
		if (Key::Check(Key_SET) != KeyStatus_None) {
			//设置键二次按下，结束输入
			return;
		} else if ((status = Key::Check(Key_UP, wait)) != KeyStatus_None) {
			//++
			if (*num < upper) {
				++*num;
			}
			Display::Num(*num, isTemp, led);
			last = millis();
			if (status == KeyStatus_LoogPress) {
				if (loogPressCount[0] >= 50) {
					wait = 10;
				} else if (loogPressCount[0] >= 8) {
					//长期长按，进入快速模式
					wait = 50;
					loogPressCount[0]++;
				} else {
					loogPressCount[0]++;
				}
				continue;
			}
		} else if ((status = Key::Check(Key_DOWN, wait)) != KeyStatus_None) {
			//--
			if (*num > lower) {
				--*num;
			}
			Display::Num(*num, isTemp, led);
			last = millis();
			if (status == KeyStatus_LoogPress) {
				if (loogPressCount[1] >= 50) {
					wait = 10;
				} else if (loogPressCount[1] >= 8) {
					//长期长按，进入快速模式
					wait = 50;
					loogPressCount[1]++;
				} else {
					loogPressCount[1]++;
				}
				continue;
			} else if ((millis() - last) > KeyPressLimit) {
				//按键输入超时
				*num = 9999;
				return;
			}
		}
		loogPressCount[0] = 0;
		loogPressCount[1] = 0;
		wait = 250;
	}
}
