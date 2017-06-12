/*
 * U_I2C1.h
 *
 *  Created on: 2016��2��7��
 *      Author: Romeli
 *      Used Pin: 	|  PB6-I2C1_SCL	  |
 *          		|  PB7-I2C1_SDA   |
 */

#ifndef U_I2C1_H_
#define U_I2C1_H_

#include "stm32f10x.h"

#define I2CSpeed 400000
#define SlaveAddress 0x44

class U_I2C1Class {
public:
	U_I2C1Class() {
		I2C_DeInit(I2C1);
		GPIOInit();
		I2CInit();
		NVICInit();
	}
	void SendAsync(uint8_t D_Add, uint8_t W_Add, uint8_t data);
	void SendAsync(uint8_t D_Add, uint8_t W_Add, uint8_t *dataBuf,
			uint8_t size);

	uint8_t ReceiveSync(uint8_t D_Add, uint8_t R_Add);
	void ReceiveSync(uint8_t D_Add, uint8_t R_Add, uint8_t *dataBuf,
			uint8_t size);
private:
	void GPIOInit();
	void I2CInit();
	void NVICInit();
	void ExitBusy();
};

extern U_I2C1Class U_I2C1;
#endif /* U_I2C1_H_ */
