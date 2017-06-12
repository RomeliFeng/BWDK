/*
 * Door.h
 *
 *  Created on: 2017年5月10日
 *      Author: Romeli
 */

#ifndef DOOR_H_
#define DOOR_H_

#include "cmsis_device.h"

class Door {
public:
	static void Init();
	static bool Check();
private:
	static void GPIOInit();
};

#endif /* DOOR_H_ */
