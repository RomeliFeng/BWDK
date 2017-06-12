/*
 * Buzzer.h
 *
 *  Created on: 2017年5月10日
 *      Author: Romeli
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "cmsis_device.h"

class Buzzer {
public:
	static void Init();
	static void Control(Status_Typedef status);
private:
	static void GPIOInit();
};

#endif /* BUZZER_H_ */
