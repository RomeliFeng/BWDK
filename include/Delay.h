/*
 * Delay.h
 *
 *  Created on: 2016Äê2ÔÂ6ÈÕ
 *      Author: Romeli
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "stm32f10x.h"

extern void Delay_Init(uint16_t DelayResolution_us);
extern void Delay_us(uint64_t us);
extern inline void Delay_ms(uint32_t ms) {
	Delay_us(ms * 1000);
}
extern inline void Delay_s(uint32_t s) {
	Delay_us(s * 1000000);
}
extern uint32_t millis();
extern uint64_t micros();

#endif /* DELAY_H_ */
