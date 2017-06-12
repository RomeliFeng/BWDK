/*
 * TimeTick.h
 *
 *  Created on: 2017��1��11��
 *      Author: Romeli
 */

#ifndef TIMETICK_H_
#define TIMETICK_H_

#include "cmsis_device.h"

class TimeTickClass {
public:
	TimeTickClass() {
		TIMInit();
		NVICInit();
	}
	bool ThreadStart;
private:
	void TIMInit();
	void NVICInit();
};

extern void TimeTickISR();
extern TimeTickClass TimeTick;
#endif /* TIMETICK_H_ */
