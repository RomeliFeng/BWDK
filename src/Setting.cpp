/*
 * Setting.cpp
 *
 *  Created on: 2017年5月9日
 *      Author: Romeli
 */

#include "Setting.h"
#include "Temp.h"

#define SettingAdd 0x0800f000

#define TempAdd 0x0800f400

uint32_t TempSP = 0x0800f400;

Setting_Typedef MainSetting;

void Setting::Load() {
	uint8_t* p = (uint8_t*) SettingAdd;

	if (*p == 0xff) {
		//未发现设置，初始化并保存
		MainSetting.setting.S_BaudRate = 9600;
		MainSetting.setting.S_USART_Parity = USART_Parity_No;
		MainSetting.setting.S_OB = 900;
		MainSetting.setting.S_DF = 100;
		MainSetting.setting.S_AH = 1500;
		MainSetting.setting.S_AL = 1200;
		MainSetting.setting.S_Add = 1;
		MainSetting.setting.S_T[0] = 0;
		MainSetting.setting.S_T[1] = 0;
		MainSetting.setting.S_T[2] = 0;
		MainSetting.setting.S_T[3] = 0;
		Save(&MainSetting);
	} else {
		for (uint8_t i = 0; i < sizeof(MainSetting) / 2; i++) {
			MainSetting.halfWord[i] = *(uint16_t*) (SettingAdd + (i * 2));
		}
	}

	//开启芯片的读保护
#ifdef EnableReadProtect
	FLASH_Unlock();
	FLASH_ReadOutProtection(ENABLE);
	FLASH_Lock();
#endif
}

void Setting::Save(Setting_Typedef* settingSave) {
	__disable_irq();
	FLASH_Unlock();

	FLASH_ErasePage(SettingAdd);
	for (uint8_t i = 0; i < sizeof(MainSetting) / 2; i++) {
		FLASH_ProgramHalfWord(SettingAdd + (i * 2), settingSave->halfWord[i]);
	}

	FLASH_Lock();
	__enable_irq();
}

//计算上次温度指针
void Setting::Ready() {
	uint16_t i;
	__disable_irq();
	for (i = 0; i < 0x400; i += 8) {
		if (*(uint16_t*) (TempAdd + i) == 0xffff) {
			break;
		}
	}
	if (i == 0) {
		TempSP = TempAdd;
		FLASH_Unlock();

		FLASH_ErasePage(TempAdd);
		FLASH_ProgramHalfWord(TempAdd, (uint16_t) Temp::Data[0]);
		FLASH_ProgramHalfWord(TempAdd + 2, (uint16_t) Temp::Data[1]);
		FLASH_ProgramHalfWord(TempAdd + 4, (uint16_t) Temp::Data[2]);
		FLASH_ProgramHalfWord(TempAdd + 6, (uint16_t) Temp::Data[3]);

		FLASH_Lock();
	} else if (i == 0x400) {
		uint16_t tmp[4];
		tmp[0] = *(uint16_t*) (TempAdd + i - 8);
		tmp[1] = *(uint16_t*) (TempAdd + i - 6);
		tmp[2] = *(uint16_t*) (TempAdd + i - 4);
		tmp[3] = *(uint16_t*) (TempAdd + i - 2);

		FLASH_Unlock();

		FLASH_ErasePage(TempAdd);
		FLASH_ProgramHalfWord(TempAdd, tmp[0]);
		FLASH_ProgramHalfWord(TempAdd + 2, tmp[1]);
		FLASH_ProgramHalfWord(TempAdd + 4, tmp[2]);
		FLASH_ProgramHalfWord(TempAdd + 6, tmp[3]);

		FLASH_Lock();
	} else {
		TempSP = TempAdd + i - 8;
	}
	__enable_irq();
}

void Setting::SaveTemp() {
	__disable_irq();

	FLASH_Unlock();

	FLASH_ProgramHalfWord(TempSP + 8, (uint16_t) Temp::Data[0]);
	FLASH_ProgramHalfWord(TempSP + 10, (uint16_t) Temp::Data[1]);
	FLASH_ProgramHalfWord(TempSP + 12, (uint16_t) Temp::Data[2]);
	FLASH_ProgramHalfWord(TempSP + 14, (uint16_t) Temp::Data[3]);
	FLASH_Lock();
	__enable_irq();
}
