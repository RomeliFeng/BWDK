/*
 * PVD.h
 *
 *  Created on: 2017年5月14日
 *      Author: Romeli
 */

#ifndef PVD_H_
#define PVD_H_

#include "cmsis_device.h"

class PVD {
public:
	static void Init();
private:
	static void GPIOInit();
	static void EXTIInit();
	static void NVICInit();
	static void PVDInit();
};

extern void PowerDownEvent();
#endif /* PVD_H_ */
