/*
 * Key.h
 *
 *  Created on: 2017年5月10日
 *      Author: Romeli
 */

#ifndef KEY_H_
#define KEY_H_

#include "cmsis_device.h"
#include "Display.h"

#define KeyPressLimit 60000

typedef enum _Key_Typedef {
	Key_SET, Key_UP, Key_DOWN, Key_SILENCE, Key_ESC
} Key_Typedef;

typedef enum _KeyStatus_Typedef {
	KeyStatus_Press, KeyStatus_LoogPress, KeyStatus_None
} KeyStatus_Typedef;

class Key {
public:
	static KeyStatus_Typedef Check(Key_Typedef key, uint16_t wait = 0);
	static void Wait(Key_Typedef key, uint16_t wait = KeyPressLimit);
	static void Input(P_Typedef p, int16_t* num, int16_t lower, int16_t upper,
			bool isTemp = false, uint8_t led = 0x00);
};

#endif /* KEY_H_ */
