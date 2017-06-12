/*
 * Relay.cpp
 *
 *  Created on: 2017年5月7日
 *      Author: Romeli
 */

#include "Relay.h"
#include "TM16xx.h"

#define SP1_PIN GPIO_Pin_11
#define AH_PIN GPIO_Pin_12
#define AL_PIN GPIO_Pin_13
#define ER_PIN GPIO_Pin_14
#define CN_PIN GPIO_Pin_15

BytetoBit_Typedef RelayStatus = { 0 };

void Relay::Control(RelayCh_Typedef ch, Status_Typedef status) {
	switch (ch) {
	case RelayCh_AH:
		if (status == Status_On) {
			TM1668.Data[10] |= 0x02;
			GPIO_SetBits(GPIOB, AH_PIN);
		} else {
			TM1668.Data[10] &= ~0x02;
			GPIO_ResetBits(GPIOB, AH_PIN);
		}
		break;
	case RelayCh_AL:
		if (status == Status_On) {
			TM1668.Data[10] |= 0x04;
			GPIO_SetBits(GPIOB, AL_PIN);
		} else {
			TM1668.Data[10] &= ~0x04;
			GPIO_ResetBits(GPIOB, AL_PIN);
		}
		break;
	case RelayCh_ER:
		if (status == Status_On) {
			TM1668.Data[10] |= 0x20;
			GPIO_SetBits(GPIOB, ER_PIN);
		} else {
			TM1668.Data[10] &= ~0x20;
			GPIO_ResetBits(GPIOB, ER_PIN);
		}
		break;
	case RelayCh_CN:
		if (status == Status_On) {
			TM1668.Data[10] |= 0x01;
			GPIO_SetBits(GPIOB, CN_PIN);
		} else {
			TM1668.Data[10] &= ~0x01;
			GPIO_ResetBits(GPIOB, CN_PIN);
		}
		break;
	case RelayCh_SP1:
		break;
	default:
		break;
	}
	if (status == Status_On) {
		RelayStatus.byte |= (1 << ch);
	} else {
		RelayStatus.byte &= ~(1 << ch);
	}
}

void Relay::GPIOInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = SP1_PIN | AH_PIN | AL_PIN | ER_PIN | CN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
