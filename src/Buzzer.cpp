/*
 * Buzzer.cpp
 *
 *  Created on: 2017年5月10日
 *      Author: Romeli
 */

#include "Buzzer.h"

void Buzzer::Init() {
	Buzzer::GPIOInit();
}

void Buzzer::Control(Status_Typedef status) {
	if (status != Status_Off) {
		GPIO_SetBits(GPIOB, GPIO_Pin_10);
	} else {
		GPIO_ResetBits(GPIOB, GPIO_Pin_10);
	}
}

void Buzzer::GPIOInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
