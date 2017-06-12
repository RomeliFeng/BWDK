/*
 * Temp.h
 *
 *  Created on: 2017年5月3日
 *      Author: Romeli
 */

#ifndef TEMP_H_
#define TEMP_H_

#include "cmsis_device.h"

#define TempChNum 3

#define TempOPLimit 3000
#define TempUpeerLimit 2400
#define TempLowerLimit 0

#define TempOP 0x7000
#define TempUpeer 0x6000
#define TempLower 0x8000

typedef enum _TempCh_Typedef {
	TempCh_0, TempCh_1, TempCh_2, TempCh_3
} TempCh_Typedef;

class Temp {
public:
	static int32_t Data[4];
	static bool CN[4];
	static bool AH[4];
	static bool AL[4];
	static bool Error[4];

	static void Init();
	static void RefreshData();
	static void DisplayTemp(TempCh_Typedef ch);
private:
	static uint8_t CNConut[4];
	static uint8_t AHConut[4];
	static uint8_t ALConut[4];

	static int32_t DataFIFO[4][8];
	static uint8_t DataFIFOSP;

	static void GPIOInit();
	static uint16_t ConvertToRes(uint16_t adc);
	static uint16_t ResCalib(TempCh_Typedef ch, uint16_t res);
	static int16_t ConvertToTemp(uint16_t res);

	static void Sample();
	static void Check(TempCh_Typedef ch, int32_t temp);
	static int cmp(const void *a, const void *b);
};
#endif /* TEMP_H_ */
