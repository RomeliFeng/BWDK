/*
 * U_USART1.h
 *
 *  Created on: 2016��1��18��
 *      Author: Romeli
 *      Version: V1.0
 */

#ifndef U_USART1_H_
#define U_USART1_H_

#include "cmsis_device.h"
#include "Parse.h"

#define USE_DMA

#define USART1_TX_Buf_Size 256
#define USART1_RX_Buf_Size 256

static uint8_t NewLine[] = { '\r', '\n' };

class SerialClass: public Parse {
public:
	void begin(uint32_t BaudRate, uint16_t USART_Parity);

	void print(int32_t data, uint8_t base = 10);

	inline void print(int8_t data, uint8_t base = 10) {
		print((int32_t) data, base);
	}
	inline void print(int16_t data, uint8_t base = 10) {
		print((int32_t) data, base);
	}
	inline void print(int64_t data, uint8_t base = 10) {
		print((int32_t) data, base);
	}

	inline void print(uint8_t data, uint8_t base = 10) {
		print((int32_t) data, base);
	}
	inline void print(uint16_t data, uint8_t base = 10) {
		print((int32_t) data, base);
	}
	inline void print(uint32_t data, uint8_t base = 10) {
		print((int32_t) data, base);
	}
	inline void print(uint64_t data, uint8_t base = 10) {
		print((int32_t) data, base);
	}

	void print(double data, uint8_t ndigit = 2);
	void print(float data, uint8_t ndigit = 2) {
		print((double) data, ndigit);
	}

	void println() {
		print(NewLine, 2);
	}
	inline void println(int8_t *data) {
		print((uint8_t *) data);
		println();
	}
	inline void println(uint8_t *data) {
		print(data);
		println();
	}
	inline void println(const char *data) {
		print((uint8_t *) data);
		println();
	}

	inline void println(int32_t data, uint8_t base = 10) {
		print(data, base);
		println();
	}

	inline void println(int8_t data, uint8_t base = 10) {
		println((int32_t) data, base);
	}
	inline void println(int16_t data, uint8_t base = 10) {
		println((int32_t) data, base);
	}
	inline void println(int64_t data, uint8_t base = 10) {
		println((int32_t) data, base);
	}

	inline void println(uint8_t data, uint8_t base = 10) {
		println((int32_t) data, base);
	}
	inline void println(uint16_t data, uint8_t base = 10) {
		println((int32_t) data, base);
	}
	inline void println(uint32_t data, uint8_t base = 10) {
		println((int32_t) data, base);
	}
	inline void println(uint64_t data, uint8_t base = 10) {
		println((int32_t) data, base);
	}

	inline void println(double data, uint8_t ndigit = 2) {
		print(data, ndigit);
		println();
	}
	inline void println(float data, uint8_t ndigit = 2) {
		print((double) data, ndigit);
	}

	void print(uint8_t *data) {
		print(data, getlen(data));
	}
	inline void print(char *data) {
		print((uint8_t *) data, getlen((uint8_t *) data));
	}
	inline void print(const char *data) {
		print((uint8_t *) data, getlen((uint8_t *) data));
	}

	void print(uint8_t* data, uint16_t len);
	int32_t nextInt();
	double nextFloat();
	void write(uint8_t);
	uint16_t available();
	bool checkFrame();
	bool checkBusy();
	uint8_t peek();
	uint8_t peekNextDigit(bool detectDecimal = false);
	uint8_t read();
	void read(uint8_t* buf, uint16_t len);
	void clear();
private:
	void DMASend(uint8_t ch);
	uint16_t getlen(uint8_t* data);
	void ReadSPInc();
	void ReadSPDec();
};

extern SerialClass Serial;
#endif /* U_USART1_H_ */
