/*
 * Parse.cpp
 *
 *  Created on: 2016��10��6��
 *      Author: Romeli
 */

#include "Parse.h"

unsigned char Parse::byNumber(unsigned long num, unsigned char base,
		unsigned char* str) {
	unsigned char len = getLen(num, base);
	str += len;
	*str = '\0';
	do {
		char c = num % base;
		num /= base;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	} while (num);
	return len;
}

unsigned char Parse::byNumber(long num, unsigned char base,
		unsigned char* str) {
	if (num < 0) {
		num = -num;
		*str++ = '-';
	}
	return byNumber((unsigned long) num, base, str);
}

unsigned char Parse::byFloat(double flo, unsigned char ndigit,
		unsigned char* str) {
	unsigned char len = 0, len2 = 0;
	unsigned long int_part = 0;
	double rem_part = 0;

	unsigned char dot = '.';

	unsigned char tmp[20];
	// Handle negative flos
	if (flo < 0.0) {
		str[len] = '-';
		flo = -flo;
		len++; //字符串长度+1 ‘-’号长度
	}

	// Round correctly so that print(1.999, 2) prints as "2.00"
	double rounding = 0.5;
	for (unsigned char i = 0; i < ndigit; ++i)
		rounding /= 10.0;

	flo += rounding;

	// Extract the integer part of the number and print it
	int_part = (unsigned long) flo;
	rem_part = (flo - (double) int_part);

	len2 = byNumber(int_part, 10, tmp); //转换整数部分
	len = strcat(str, len, tmp, len2); //拼接整数部分到字符串
	if (ndigit > 0) { //如果有小数部分
		len = strcat(str, len, &dot, 1);
		while (ndigit--) {
			rem_part *= 10;
			int_part = (int) rem_part;	//每次转换一位小数
			rem_part -= int_part;
			str[len++] = int_part + 0x30;
		}
	}
	return len;
}

unsigned char Parse::getLen(unsigned long num, unsigned char base) {
	unsigned char i;
	if (num == 0) {
		return 1;
	}
	for (i = 0; num != 0; ++i) {
		num /= base;
	}
	return i;
}

double Parse::pow10(unsigned char power) {
	if (power == 0)
		return 1.0;
	else
		return 10.0 * pow10(--power);
}

unsigned char Parse::strcat(unsigned char* str_to, unsigned char str_to_len,
		unsigned char* str_from, unsigned char str_from_len) {
	unsigned char i;
	for (i = 0; i < str_from_len; ++i) { //搬移数据
		str_to[str_to_len + i] = str_from[i];
	}
	str_to[str_to_len + i] = '\0'; //在字符串末尾填'\0'
	return str_to_len + str_from_len; //返回字符串长度
}
