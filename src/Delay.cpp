/*
 * Delay.cpp
 *
 *  Created on: 2016��2��6��
 *      Author: Romeli
 */

#include "Delay.h"

static volatile uint_fast64_t nTime_now = 0;
uint_fast64_t nTime_last;
uint16_t Interval = 0;

void Delay_Init(uint16_t DelayResolution_us) {
	Interval = DelayResolution_us;
	SysTick_Config((SystemCoreClock / (1000000 / Interval)) - 5); //Set SysTick timer=us
	NVIC_SetPriority(SysTick_IRQn, 0);					//Set SysTick interrupt
//	SysTick->CTRL&=SysTick_CTRL_ENABLE_Msk;			//Disable SysTick
}

void Delay_us(uint64_t us) {
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;			//Ensable SysTick
	nTime_last = nTime_now;							//Record time_now
	while ((nTime_now - nTime_last) < us)
		;				//keep loop when in delay time
}

uint32_t millis() {
	return nTime_now / 1000;
}

uint64_t micros() {
	return nTime_now;
}

extern "C" void SysTick_Handler() {
	nTime_now += Interval;
}
