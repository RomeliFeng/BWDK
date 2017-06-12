/*
 * Door.cpp
 *
 *  Created on: 2017年5月10日
 *      Author: Romeli
 */

#include "Door.h"

void Door::Init() {
	Door::GPIOInit();
}

bool Door::Check() {
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) != Bit_RESET) {
		return true;
	} else {
		return false;
	}
}

void Door::GPIOInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
