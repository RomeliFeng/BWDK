#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

#include "cmsis_device.h"

#include "TM16xx.h"
#include "Temp.h"
#include "Delay.h"
#include "TimeTick.h"
#include "U_USART1.h"
#include "Relay.h"
#include "Display.h"
#include "Key.h"
#include "LED.h"
#include "Door.h"
#include "PVD.h"
#include "Buzzer.h"

#include "Setting.h"

#define TempSwitchDelay 3000
#define TempRefreshInterval 400
#define BuzzerRefreshInterval 300
#define SilenceDelay 600000

typedef enum _Mode_Typedef {
	Mode_Max, Mode_Round
} Mode_Typedef;

void ModuleInit();
void BuzzerAndDoorEvent();
void RelayEvent();
void DisplayEvent();
void KeyEvent();

void SettingSet();

bool HandMode = false;
bool AutoOnCN = false;
bool NoBuzzer = false;
bool DoorOpen = false;

bool InitOk = false;

uint32_t BuzzerTime;
uint32_t SaftyDelay; //传感器断线后上电瞬间延时保护

Mode_Typedef CurrentMode = Mode_Round;

int main(int argc, char* argv[]) {
	//系统模块初始化
	ModuleInit();
	while (true) {
		//蜂鸣器事件
		BuzzerAndDoorEvent();
		//继电器状态切换事件
		RelayEvent();
		//按键按下事件
		KeyEvent();
		//显示刷新
		DisplayEvent();
	}
}

void ModuleInit() {
	//初始化显示
	TM1668.Init();
	Display::P(P_LEAD);
	TM1668.RefreshData();

	//从flash读取数据
	Setting::Load();
	Setting::Ready();

	//初始化时基
	Delay_Init(10);
	//初始化串口
	Serial.begin(MainSetting.setting.S_BaudRate,
			MainSetting.setting.S_USART_Parity);

	//初始化温度
	Temp::Init();
	Temp::RefreshData();

	//初始化继电器
	Relay::Init();
	//初始化门检测
	Door::Init();
	//初始化蜂鸣器
	Buzzer::Init();
	//初始化掉点监测
	PVD::Init();
	//默认Round模式，开启指示灯
	LED::Control(LED_Round, Status_On);

	//开启温度和显示刷新
	TimeTick.ThreadStart = true;
	//防止未初始化完成奖就保存数据
	InitOk = true;

	TM1668.Data[0] = 0x01;
	Delay_s(1);
	TM1668.Data[0] = 0x02;
	Delay_s(1);
	TM1668.Data[0] = 0x04;
	Delay_s(1);
}

void BuzzerAndDoorEvent() {
	bool TempError = Temp::Error[0] || Temp::Error[1] || Temp::Error[2]
			|| Temp::Error[3];
	bool TempAL = Temp::AL[0] || Temp::AL[1] || Temp::AL[2] || Temp::AL[3];
	bool TempAH = Temp::AH[0] || Temp::AH[1] || Temp::AH[2] || Temp::AH[3];

	static uint32_t last = millis();
	static bool blink = false;

	DoorOpen = Door::Check();
	//检测到门信号或传感器开路时打开蜂鸣器
	if (DoorOpen) {
		if (millis() - last > 1000) {
			last = millis();
			if (blink) {
				if (!NoBuzzer) {
					Buzzer::Control(Status_On);
				}
				blink = false;
			} else {
				Buzzer::Control(Status_Off);
				blink = true;
			}
		}
	} else if (TempError || TempAH) {
		static uint8_t count = 0;
		if (millis() - last > 150) {
			last = millis();
			if (blink) {
				if (!NoBuzzer && (count <= 2)) {
					Buzzer::Control(Status_On);
				}
				if (count >= 4) {
					count = 0;
				}
				blink = false;
			} else {
				Buzzer::Control(Status_Off);
				blink = true;
				count++;
			}
		}
	} else if (TempAL) {
		if (millis() - last > 400) {
			last = millis();
			if (blink) {
				if (!NoBuzzer) {
					Buzzer::Control(Status_On);
				}
				blink = false;
			} else {
				Buzzer::Control(Status_Off);
				blink = true;
			}
		}
	} else {
		Buzzer::Control(Status_Off);
	}
}

void RelayEvent() {
	bool TempError = Temp::Error[0] || Temp::Error[1] || Temp::Error[2]
			|| Temp::Error[3];
	bool TempAL = Temp::AL[0] || Temp::AL[1] || Temp::AL[2] || Temp::AL[3];
	bool TempAH = Temp::AH[0] || Temp::AH[1] || Temp::AH[2] || Temp::AH[3];
	bool TempCN = Temp::CN[0] || Temp::CN[1] || Temp::CN[2] || Temp::CN[3];
	if (TempError) {
		Relay::Control(RelayCh_ER, Status_On);
		Relay::Control(RelayCh_CN, Status_Off);
		Relay::Control(RelayCh_AL, Status_Off);
		Relay::Control(RelayCh_AH, Status_Off);
	} else {
		Relay::Control(RelayCh_ER, Status_Off);
		if (TempAL) {
			Relay::Control(RelayCh_AL, Status_On);
		} else {
			Relay::Control(RelayCh_AL, Status_Off);
		}
		if (TempAH) {
			Relay::Control(RelayCh_AH, Status_On);
		} else {
			Relay::Control(RelayCh_AH, Status_Off);
		}
		if (TempCN) {
			Relay::Control(RelayCh_CN, Status_On);
			AutoOnCN = true; //锁定风扇继电器状态，防止被手动关闭
		} else {
			if (!HandMode) { //查询继电器是否手动打开
				Relay::Control(RelayCh_CN, Status_Off);
			}
			AutoOnCN = false; //解锁风扇继电器状态
		}
	}

}

void DisplayEvent() {
	static uint32_t last = millis();
	static uint32_t last2 = millis();

	if (DoorOpen) {
		Display::P(P_Door);
	} else if (CurrentMode == Mode_Max) {
		//判断是否是最大温度显示模式
		uint8_t j = 0;
		//寻找温度最高的通道
		for (uint8_t i = 0; i < TempChNum; ++i) {
			if (Temp::Data[i] > Temp::Data[j]) {
				j = i;
			}
		}
		//按照刷新间隔刷新温度
		if (millis() - last2 > TempRefreshInterval) {
			last2 = millis();
			static bool blink = false;
			if (blink) {
				Display::P(P_NONE, 1 << j);
				blink = false;
			} else {
				Temp::DisplayTemp((TempCh_Typedef) j);
				if (Temp::Error[j] || (Temp::AH[j])) {
					blink = true;
				}
			}
		}
	} else if ((millis() - last) < TempSwitchDelay) {
		//第一通道温度显示
		if (millis() - last2 > TempRefreshInterval) {
			last2 = millis();
			static bool blink = false;
			if (blink) {
				Display::P(P_NONE, 1 << TempCh_0);
				blink = false;
			} else {
				Temp::DisplayTemp(TempCh_0);
				if (Temp::Error[TempCh_0] || (Temp::AH[TempCh_0])) {
					blink = true;
				}
			}
		}
	} else if ((millis() - last) < (TempSwitchDelay * 2)) {
		//第二通道温度显示
		if (millis() - last2 > TempRefreshInterval) {
			last2 = millis();
			static bool blink = false;

			if (blink) {
				Display::P(P_NONE, 1 << TempCh_1);
				blink = false;
			} else {
				Temp::DisplayTemp(TempCh_1);
				if (Temp::Error[TempCh_1] || (Temp::AH[TempCh_1])) {
					blink = true;
				}
			}
		}
	} else if ((millis() - last) < (TempSwitchDelay * 3)) {
		//第三通道温度显示
		if (millis() - last2 > TempRefreshInterval) {
			last2 = millis();
			static bool blink = false;
			if (blink) {
				Display::P(P_NONE, 1 << TempCh_2);
				blink = false;
			} else {
				Temp::DisplayTemp(TempCh_2);
				if (Temp::Error[TempCh_2] || (Temp::AH[TempCh_2])) {
					blink = true;
				}
			}
		}
	} else if ((TempChNum == 4)
			&& ((millis() - last) < (TempSwitchDelay * 4))) {
		//第四通道温度显示；默认关闭
		if (millis() - last2 > TempRefreshInterval) {
			last2 = millis();
			static bool blink = false;
			if (blink) {
				Display::P(P_NONE, 1 << TempCh_3);
				blink = false;
			} else {
				Temp::DisplayTemp(TempCh_3);
				if (Temp::Error[TempCh_3] || (Temp::AH[TempCh_3])) {
					blink = true;
				}
			}
		}
	} else {
		//重置轮询显示计时
		last = millis();
	}
}

void KeyEvent() {
	KeyStatus_Typedef key;
	if ((key = Key::Check(Key_SET)) != KeyStatus_None) {
		//设置键按下，设置指示灯
		LED::Control(LED_Round, Status_On);
		LED::Control(LED_Max, Status_On);
		//进入菜单
		SettingSet();
		//撤销指示灯
		if (CurrentMode == Mode_Round) {
			LED::Control(LED_Round, Status_On);
			LED::Control(LED_Max, Status_Off);
		} else if (CurrentMode == Mode_Max) {
			LED::Control(LED_Round, Status_Off);
			LED::Control(LED_Max, Status_On);
		}
	} else if ((key = Key::Check(Key_UP)) != KeyStatus_None) {
		//上键按下，判断是否为手动模式
		if (LED::Status(LED_Hand) != Status_Off) {
			//退出手动模式，判断风扇继电器是否为自动被开启
			if (!AutoOnCN) {
				Relay::Control(RelayCh_CN, Status_Off);
			}
			LED::Control(LED_Hand, Status_Off);
			HandMode = false;
		} else {
			//进入手动模式，开启风扇继电器
			Relay::Control(RelayCh_CN, Status_On);
			LED::Control(LED_Hand, Status_On);
			HandMode = true;
		}
	} else if ((key = Key::Check(Key_DOWN)) != KeyStatus_None) {
		//下键被按下
		if (CurrentMode == Mode_Max) {
			//进入轮询模式
			LED::Control(LED_Round, Status_On);
			LED::Control(LED_Max, Status_Off);
			CurrentMode = Mode_Round;
		} else {
			//进入最大温度显示模式
			LED::Control(LED_Round, Status_Off);
			LED::Control(LED_Max, Status_On);
			CurrentMode = Mode_Max;
		}
	} else if ((key = Key::Check(Key_SILENCE)) != KeyStatus_None) {
		NoBuzzer = true;
		BuzzerTime = millis();
	} else if ((key = Key::Check(Key_ESC, 10000)) != KeyStatus_None) {
		if (key == KeyStatus_Press) {
			NVIC_SystemReset();
		} else if (key == KeyStatus_LoogPress) {
			for (uint8_t i = 0; i < 14; i++) {
				TM1668.Data[i] = 0xff;
			}
			TM1668.RefreshData();
#ifdef EnableReadProtect
			FLASH_Unlock();
			FLASH_ReadOutProtection(DISABLE);
			FLASH_Lock();
#endif
		}
	} else {
		if (NoBuzzer) {
			if (millis() - BuzzerTime >= SilenceDelay) {
				NoBuzzer = false;
			}
		}
	}
}

void SettingSet() {
//设置功能码
	int16_t cd = 1000;
	Key::Input(P_CD, &cd, 1000, 1100);
	if (cd == 9999) {
		return;
	}

	switch (cd) {
	case 1005: {
		int16_t ob = MainSetting.setting.S_OB;
		int16_t df = MainSetting.setting.S_DF;
		int16_t ah = MainSetting.setting.S_AH;
		int16_t al = MainSetting.setting.S_AL;
		//设置OB
		Key::Input(P_OB, &ob, -400, 3000, true);
		//设置DF
		Key::Input(P_DF, &df, 1, 3000, true);
		//设置AH
		Key::Input(P_AH, &ah, -400, 3000, true);
		//设置AL
		Key::Input(P_AL, &al, -400, 3000, true);

		if ((ob == 9999) || (df == 9999) || (ah == 9999) || (al == 9999)) {
			return;
		}

		MainSetting.setting.S_OB = ob;
		MainSetting.setting.S_DF = df;
		MainSetting.setting.S_AH = ah;
		MainSetting.setting.S_AL = al;
		break;
	}
	case 1008: {
		int16_t ta = MainSetting.setting.S_T[TempCh_0];
		int16_t tb = MainSetting.setting.S_T[TempCh_1];
		int16_t tc = MainSetting.setting.S_T[TempCh_2];
		int16_t td = MainSetting.setting.S_T[TempCh_3];
		//设置TA
		Key::Input(P_AD, &ta, -199, 199, true);
		//设置TB
		Key::Input(P_AD, &tb, -199, 199, true);
		//设置TC
		Key::Input(P_AD, &tc, -199, 199, true);
		//设置TD
//		Key::Input(P_AD, &td, -199, 199, true);
		if ((ta == 9999) || (tb == 9999) || (tc == 9999) || (td == 9999)) {
			return;
		}

		MainSetting.setting.S_T[TempCh_0] = ta;
		MainSetting.setting.S_T[TempCh_1] = tb;
		MainSetting.setting.S_T[TempCh_2] = tc;
//		MainSetting.setting.S_T[TempCh_3] = td;
		break;
	}
	case 1010: {
		int16_t add = MainSetting.setting.S_Add;
		Key::Input(P_DD, &add, 1, 28);
		if (add == 9999) {
			return;
		}

		MainSetting.setting.S_Add = add;
		break;
	}
	case 1013: {
		int16_t pc;
		int16_t bl;

		if (MainSetting.setting.S_USART_Parity == USART_Parity_No) {
			pc = 0;
		} else if (MainSetting.setting.S_USART_Parity == USART_Parity_Even) {
			pc = 1;
		} else {
			pc = 2;
		}
		if (MainSetting.setting.S_BaudRate == 1200) {
			bl = 0;
		} else if (MainSetting.setting.S_BaudRate == 2400) {
			bl = 1;
		} else if (MainSetting.setting.S_BaudRate == 4800) {
			bl = 2;
		} else {
			bl = 3;
		}
		//设置PC
		Key::Input(P_PC, &pc, 0, 2);
		//设置BL
		Key::Input(P_BL, &bl, 0, 3);

		if ((pc == 9999) || (bl == 9999)) {
			return;
		}

		switch (pc) {
		case 0:
			MainSetting.setting.S_USART_Parity = USART_Parity_No;
			break;
		case 1:
			MainSetting.setting.S_USART_Parity = USART_Parity_Even;
			break;
		case 2:
			MainSetting.setting.S_USART_Parity = USART_Parity_Odd;
			break;
		default:
			break;
		}

		switch (bl) {
		case 0:
			MainSetting.setting.S_BaudRate = 1200;
			break;
		case 1:
			MainSetting.setting.S_BaudRate = 2400;
			break;
		case 2:
			MainSetting.setting.S_BaudRate = 4800;
			break;
		case 3:
			MainSetting.setting.S_BaudRate = 9600;
			break;
		default:
			break;
		}

		while (Serial.checkBusy())
			;
		Serial.begin(MainSetting.setting.S_BaudRate,
				MainSetting.setting.S_USART_Parity);
		break;
	}

	default:
		break;
	}
	Setting::Save(&MainSetting);
}

void TimeTickISR() {
	//定时刷新温度、显示和键盘
	static uint8_t count = 0;
	//刷新显示和键盘  耗时1.12mS
	TM1668.RefreshData();
	count++;
	if (count >= 6) {
		//采集温度，耗时53mS
		Temp::RefreshData();
		count = 0;
	}
}

void PowerDownEvent() {
	Relay::Control(RelayCh_CN, Status_Off);
	Relay::Control(RelayCh_AL, Status_Off);
	Relay::Control(RelayCh_AH, Status_Off);
	Relay::Control(RelayCh_ER, Status_Off);
	Relay::Control(RelayCh_SP1, Status_Off);

	Buzzer::Control(Status_Off);

	TM1668.Close();
	if (InitOk) {
		Setting::SaveTemp();
	}
}

#pragma GCC diagnostic pop
