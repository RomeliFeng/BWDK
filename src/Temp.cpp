/*
 * Temp.cpp
 *
 *  Created on: 2017年5月3日
 *      Author: Romeli
 */

#include "Temp.h"
#include "U_ADC1.h"
#include "Display.h"
#include "Setting.h"
#include "Delay.h"
#include "stdlib.h"
#include "stdio.h"

#define SaftyDelay 3000 //防止掉电瞬间后恢复时锁定状态
#define Delaying 20

#define USEFILTER

#define OverLevel 3

#define ADC_Max (U_ADC_Max << OverLevel)

#define TabStartTemp (-500)
#define BridgeGain 20.833
//#define BridgeGain 20.833
#define BridgeRef (0.024848 * ADC_Max * BridgeGain) //99.31*BridgeGain
#define BridgeVCC (ADC_Max * BridgeGain) //U_ADC_Max * BridgeGain

uint16_t ResTab[4][8] = { { 8480, 10104, 13071, 15026, 17542, 18745, 21535,
		23677 }, { 8614, 10241, 13221, 15183, 17706, 18914, 21710, 23861 }, {
		8453, 10068, 13028, 14979, 17487, 18685, 21465, 23604 }, { } };
int16_t ResTab2[4][8] = { { -123, -84, -10, 40, 107, 142, 221, 287 }, { -257,
		-221, -160, -117, -57, -27, 46, 103 }, { -96, -48, 33, 87, 162, 202,
		291, 360 }, { } };

const uint16_t PT100Tab[351] = { 8031, 8070, 8110, 8150, 8189, 8229, 8269, 8308,
		8348, 8387, 8427, 8467, 8506, 8546, 8585, 8625, 8664, 8704, 8743, 8783,
		8822, 8862, 8901, 8940, 8980, 9019, 9059, 9098, 9137, 9177, 9216, 9255,
		9295, 9334, 9373, 9412, 9452, 9491, 9530, 9569, 9609, 9648, 9687, 9726,
		9765, 9804, 9844, 9883, 9922, 9961, 10000, 10039, 10078, 10117, 10156,
		10195, 10234, 10273, 10312, 10351, 10390, 10429, 10468, 10507, 10546,
		10585, 10624, 10663, 10702, 10740, 10779, 10818, 10857, 10896, 10935,
		10973, 11012, 11051, 11090, 11129, 11167, 11206, 11245, 11283, 11322,
		11361, 11400, 11438, 11477, 11515, 11554, 11593, 11631, 11670, 11708,
		11747, 11786, 11824, 11863, 11901, 11940, 11978, 12017, 12055, 12094,
		12132, 12171, 12209, 12247, 12286, 12324, 12363, 12401, 12439, 12478,
		12516, 12554, 12593, 12631, 12669, 12708, 12746, 12784, 12822, 12861,
		12899, 12937, 12975, 13013, 13052, 13090, 13128, 13166, 13204, 13242,
		13280, 13318, 13357, 13395, 13433, 13471, 13509, 13547, 13585, 13623,
		13661, 13699, 13737, 13775, 13813, 13851, 13888, 13926, 13964, 14002,
		14040, 14078, 14116, 14154, 14191, 14229, 14267, 14305, 14343, 14380,
		14418, 14456, 14494, 14531, 14569, 14607, 14644, 14682, 14720, 14757,
		14795, 14833, 14870, 14908, 14946, 14983, 15021, 15058, 15096, 15133,
		15171, 15208, 15246, 15283, 15321, 15358, 15396, 15433, 15471, 15508,
		15546, 15583, 15620, 15658, 15695, 15733, 15770, 15807, 15845, 15882,
		15919, 15956, 15994, 16031, 16068, 16105, 16143, 16180, 16217, 16254,
		16291, 16329, 16366, 16403, 16440, 16477, 16514, 16551, 16589, 16626,
		16663, 16700, 16737, 16774, 16811, 16848, 16885, 16922, 16959, 16996,
		17033, 17070, 17107, 17143, 17180, 17217, 17254, 17291, 17328, 17365,
		17402, 17438, 17475, 17512, 17549, 17586, 17622, 17659, 17696, 17733,
		17769, 17806, 17843, 17879, 17916, 17953, 17989, 18026, 18063, 18099,
		18136, 18172, 18209, 18246, 18282, 18319, 18355, 18392, 18428, 18465,
		18501, 18538, 18574, 18611, 18647, 18684, 18720, 18756, 18793, 18829,
		18866, 18902, 18938, 18975, 19011, 19047, 19084, 19120, 19156, 19192,
		19229, 19265, 19301, 19337, 19374, 19410, 19446, 19482, 19518, 19555,
		19591, 19627, 19663, 19699, 19735, 19771, 19807, 19843, 19879, 19915,
		19951, 19987, 20023, 20059, 20095, 20131, 20167, 20203, 20239, 20275,
		20311, 20347, 20383, 20419, 20455, 20490, 20526, 20562, 20598, 20634,
		20670, 20705, 20741, 20777, 20813, 20848, 20884, 20920, 20956, 20991,
		21027, 21063, 21098, 21134, 21170, 21205 };

int32_t Temp::Data[4];
bool Temp::CN[4];
bool Temp::AH[4];
bool Temp::AL[4];
bool Temp::Error[4];

uint8_t Temp::CNConut[4];
uint8_t Temp::AHConut[4];
uint8_t Temp::ALConut[4];

int32_t Temp::DataFIFO[4][8];
uint8_t Temp::DataFIFOSP;

void Temp::Init() {
	U_ADC1.Init();
}

void Temp::RefreshData() {
	Sample();
//	U_ADC1.RefreshData(ADC_Channel_0, ADC_SampleTime_239Cycles5, OverLevel);
//	Check(TempCh_0,
//			ConvertToTemp(ResCalib(TempCh_0, ConvertToRes(U_ADC1Data)))
//					+ MainSetting.setting.S_TA);
//
//	U_ADC1.RefreshData(ADC_Channel_1, ADC_SampleTime_239Cycles5,
//	OverLevel);
//	Check(TempCh_1,
//			ConvertToTemp(ResCalib(TempCh_1, ConvertToRes(U_ADC1Data)))
//					+ MainSetting.setting.S_TB);
//
//	U_ADC1.RefreshData(ADC_Channel_2, ADC_SampleTime_239Cycles5, OverLevel);
//	Check(TempCh_2,
//			ConvertToTemp(ResCalib(TempCh_2, ConvertToRes(U_ADC1Data)))
//					+ MainSetting.setting.S_TC);
//
//	if (TempChNum == 4) {
//		U_ADC1.RefreshData(ADC_Channel_3, ADC_SampleTime_239Cycles5, OverLevel);
//		Check(TempCh_3,
//				ConvertToTemp(ConvertToRes(U_ADC1Data))
//						+ MainSetting.setting.S_TD);
//
//	}

}

void Temp::DisplayTemp(TempCh_Typedef ch) {
	uint8_t led = 1 << ch;

	if (Error[ch]) {
		if (Data[ch] == TempOP) {
			Display::P(P_OP, led);
		} else if (Data[ch] == TempUpeer) {
			Display::P(P_OH, led);
		} else if (Data[ch] == TempLower) {
			Display::P(P_OL, led);
		}
	} else {
		Display::Num(Data[ch], true, led);
	}
}

void Temp::GPIOInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2
			| GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint16_t Temp::ConvertToRes(uint16_t adc) {
	uint64_t V1 = adc + BridgeRef;
	return (V1 * 330000) / ( BridgeVCC - V1);
}

uint16_t Temp::ResCalib(TempCh_Typedef ch, uint16_t res) {
	uint8_t i;
	for (i = 0; i < 8; ++i) {
		if (res < ResTab[ch][i]) {
			break;
		}
	}
	if (i == 0) {
		return res + ResTab2[ch][i];
	} else if (i == 8) {
		return res + ResTab2[ch][i - 1];
	} else {
		int16_t diff = (float) (res - ResTab[ch][i - 1])
				/ (ResTab[ch][i] - ResTab[ch][i - 1])
				* (ResTab2[ch][i] - ResTab2[ch][i - 1]) + ResTab2[ch][i - 1];

		return res + diff;
	}
}

int16_t Temp::ConvertToTemp(uint16_t res) {
	uint16_t start, mid = 0, end;
	uint16_t data;
	int8_t floatPart = 0;
	start = 0;
	end = 350;

	//检查数据合法性
	if (res > 21205) {
		return 3000 + 200;
	} else if (res < 8031) {
		return TabStartTemp - 200;
	}

	//开始二分法查找
	while (start <= end) {
		mid = (end + start) >> 1;
		data = PT100Tab[mid];
		if (res > data) {
			start = mid + 1;
		} else if (res < data) {
			if (mid) {
				end = mid - 1;
			} else {
				break;
			}
		} else {
			break;
		}
	}
	//计算小数部分
	if (mid != 0) {
		floatPart = ((res - PT100Tab[mid]) * 10)
				/ (PT100Tab[mid] - PT100Tab[mid - 1]);
	}

	return mid * 10 + TabStartTemp + floatPart;
}

void Temp::Sample() {
#ifdef USEFILTER
	int32_t tmp[6];
	for (uint8_t ch = 0; ch < TempChNum; ch++) {
		for (uint8_t i = 0; i < 6; i++) {
			U_ADC1.RefreshData(ch, ADC_SampleTime_239Cycles5,
			OverLevel);
			tmp[i] = ConvertToTemp(
					ResCalib((TempCh_Typedef) ch, ConvertToRes(U_ADC1Data)))
					+ MainSetting.setting.S_T[ch];
		}
		qsort(tmp, 6, sizeof(tmp[0]), cmp);
		int32_t sum = 0;
		for (uint8_t i = 1; i < 5; i++) {
			sum += tmp[i];
		}
		DataFIFO[ch][DataFIFOSP] = sum / 4;
		sum = 0;
		for (uint8_t i = 0; i < 8; i++) {
			sum += DataFIFO[ch][i];
		}
		int32_t mid = sum >> 3;
		int32_t diff = DataFIFO[ch][DataFIFOSP] - (mid);
		if (diff > (mid >> 2)) {
			for (uint8_t i = 0; i < 8; i++) {
				DataFIFO[ch][i] = DataFIFO[ch][DataFIFOSP];
			}
			Check((TempCh_Typedef) ch, DataFIFO[ch][DataFIFOSP]);
		} else {
			Check((TempCh_Typedef) ch, mid);
		}
	}
	DataFIFOSP++;
	if (DataFIFOSP == 8) {
		DataFIFOSP = 0;
	}
#else
	for (uint8_t ch = 0; ch < TempChNum; ch++) {
		U_ADC1.RefreshData(ch, ADC_SampleTime_239Cycles5,
				OverLevel);
		Data[ch] = ConvertToTemp(
				ResCalib((TempCh_Typedef) ch, ConvertToRes(U_ADC1Data)))
		+ MainSetting.setting.S_T[ch];
	}
#endif
}

void Temp::Check(TempCh_Typedef ch, int32_t temp) {
	static uint32_t last = millis();
	if ((temp > TempLowerLimit) && (temp < TempUpeerLimit)) {
		Data[ch] = temp;
		Error[ch] = false;
		if (millis() - last > SaftyDelay) {
			if (temp > MainSetting.setting.S_AL + 7) {
				if (ALConut[ch] >= Delaying) {
					AL[ch] = true;
				} else {
					ALConut[ch]++;
				}
			} else if (temp < MainSetting.setting.S_AL - 3) {
				AL[ch] = false;
				ALConut[ch] = 0;
			}

			if (temp > MainSetting.setting.S_AH + 7) {
				if (AHConut[ch] >= Delaying) {
					AH[ch] = true;
				} else {
					AHConut[ch]++;
				}
			} else if (temp < MainSetting.setting.S_AH - 3) {
				AH[ch] = false;
				AHConut[ch] = 0;
			}

			if (temp > (MainSetting.setting.S_OB + MainSetting.setting.S_DF)) {
				if (CNConut[ch] >= Delaying) {
					CN[ch] = true;
				} else {
					CNConut[ch]++;
				}
			} else if (temp
					< (MainSetting.setting.S_OB - MainSetting.setting.S_DF)) {
				CN[ch] = false;
				CNConut[ch] = 0;
			}
		} else {
			AL[ch] = false;
			AH[ch] = false;
			CN[ch] = false;
			CNConut[ch] = 0;
			AHConut[ch] = 0;
			ALConut[ch] = 0;
		}
	} else {
		if (temp >= TempOPLimit) {
			Data[ch] = TempOP;
		} else if (temp >= TempUpeerLimit) {
			Data[ch] = TempUpeer;
		} else if (temp <= TempLowerLimit) {
			Data[ch] = TempLower;
		}
		last = millis();
		Error[ch] = true;
		CN[ch] = false;
		AL[ch] = false;
		AH[ch] = false;
		CNConut[ch] = 0;
		AHConut[ch] = 0;
		ALConut[ch] = 0;
	}
}

int Temp::cmp(const void *a, const void *b) {
	return (*(int *) b - *(int *) a);  //实现的是降序排序
}
