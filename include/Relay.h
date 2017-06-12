/*
 * Relay.h
 *
 *  Created on: 2017年5月7日
 *      Author: Romeli
 */

#ifndef RELAY_H_
#define RELAY_H_

#include "cmsis_device.h"

typedef enum _RelayCh_Typedef {
	RelayCh_CN, RelayCh_AH, RelayCh_AL, RelayCh_ER, RelayCh_SP1
} RelayCh_Typedef;

class Relay {
public:
	static void Init() {
		GPIOInit();
	}
	static void Control(RelayCh_Typedef ch, Status_Typedef status);
private:
	static void GPIOInit();
};

extern BytetoBit_Typedef RelayStatus;
#endif /* RELAY_H_ */
