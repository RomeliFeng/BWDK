/*
 * Parse.h
 *
 *  Created on: 2016Äê10ÔÂ6ÈÕ
 *      Author: Romeli
 */

#ifndef PARSE_H_
#define PARSE_H_

class Parse {
public:
	unsigned char byNumber(unsigned long num, unsigned char base,unsigned char* str);
	inline unsigned char byNumber(unsigned int num, unsigned char base,
			unsigned char* str) {
		return byNumber((unsigned long) num, base, str);
	}
	inline unsigned char byNumber(unsigned char num, unsigned char base,
			unsigned char* str) {
		return byNumber((unsigned long) num, base, str);
	}
	unsigned char byNumber(long num, unsigned char base,unsigned char* str);
	inline void byNumber(int num, unsigned char base,unsigned char* str) {
		byNumber((long) num, base, str);
	}
	inline void byNumber(char num, unsigned char base,unsigned char* str) {
		byNumber((long) num, base, str);
	}

	unsigned char  byFloat(double flo, unsigned char ndigit,unsigned char* str);
	inline unsigned char  byFloat(float flo, unsigned char ndigit,unsigned char* str) {
		return byFloat((double) flo, ndigit, str);
	}
private:
	unsigned char getLen(unsigned long num, unsigned char base);
	double pow10(unsigned char power);
	unsigned char strcat(unsigned char* str_to, unsigned char str_to_len,
			unsigned char* str_from, unsigned char str_from_len);
};

#endif /* PARSE_H_ */
