/*
 * Setting.h
 *
 *  Created on: 2017年5月9日
 *      Author: Romeli
 */

#ifndef SETTING_H_
#define SETTING_H_

#include "cmsis_device.h"

#define EnableReadProtect

typedef struct _SettingUnit_Typedef {
	uint32_t S_BaudRate;
	uint16_t S_USART_Parity;

	int16_t S_OB;
	int16_t S_DF;
	int16_t S_AH;
	int16_t S_AL;

	int16_t S_Add;

	int16_t S_T[4];
} SettingUnit_Typedef;

typedef union _Setting_Typedef {
	SettingUnit_Typedef setting;
	uint16_t halfWord[16];
} Setting_Typedef;

class Setting {
public:
	static void Load();
	static void Save(Setting_Typedef* setting);
	static void Ready();
	static void SaveTemp();
};

extern Setting_Typedef MainSetting;

#endif /* SETTING_H_ */
