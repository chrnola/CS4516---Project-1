#include "Utils.h"

/*
 * Author: Ray Short
 */
// convert short integer into ASCII characters
unsigned char* Utils::itoa(unsigned short n) {
	char val = 0;
	unsigned char* num = (unsigned char*) calloc(5, sizeof(unsigned char));
	if(n > 9999) {
		val = n / 10000;
		num[0] = val + 0x30;
		n -= val * 10000;
	} else num[0] = 0x30;
	
	if(n > 999) {
		val = n / 1000;
		num[1] = val + 0x30;
		n -= val * 1000;
	} else num[1] = 0x30;
	
	if(n > 99) {
		val = n / 100;
		num[2] = val + 0x30;
		n -= val * 100;
	} else num[2] = 0x30;
	
	if(n > 9) {
		val = n / 10;
		num[3] = val + 0x30;
		n -= val * 10;
	} else num[3] = 0x30;
	
	if(n > 0) {
		num[4] = n + 0x30;
	} else num[4] = 0x30;
	
	return num;
}

// Author: Chris Pinola
// convert long integer into ASCII characters
unsigned char *Utils::ltoa(long n){
	char val = 0;
	unsigned char *num = (unsigned char *) calloc(17, sizeof(unsigned char));
	if(n > 9999999999999999) {
		val = n / 10000000000000000;
		num[0] = val + 0x30;
		n -= val * 10000000000000000;
	} else num[0] = 0x30;
	
	if(n > 999999999999999) {
		val = n / 1000000000000000;
		num[1] = val + 0x30;
		n -= val * 1000000000000000;
	} else num[1] = 0x30;
	
	if(n > 99999999999999) {
		val = n / 100000000000000;
		num[2] = val + 0x30;
		n -= val * 100000000000000;
	} else num[2] = 0x30;
	
	if(n > 9999999999999) {
		val = n / 10000000000000;
		num[3] = val + 0x30;
		n -= val * 10000000000000;
	} else num[3] = 0x30;
	
	if(n > 999999999999) {
		val = n / 1000000000000;
		num[4] = val + 0x30;
		n -= val * 1000000000000;
	} else num[4] = 0x30;
	
	if(n > 99999999999) {
		val = n / 100000000000;
		num[5] = val + 0x30;
		n -= val * 100000000000;
	} else num[5] = 0x30;
	
	if(n > 9999999999) {
		val = n / 10000000000;
		num[6] = val + 0x30;
		n -= val * 10000000000;
	} else num[6] = 0x30;
	
	if(n > 999999999) {
		val = n / 1000000000;
		num[7] = val + 0x30;
		n -= val * 1000000000;
	} else num[7] = 0x30;
	
	if(n > 99999999) {
		val = n / 100000000;
		num[8] = val + 0x30;
		n -= val * 100000000;
	} else num[8] = 0x30;
	
	if(n > 9999999) {
		val = n / 10000000;
		num[9] = val + 0x30;
		n -= val * 10000000;
	} else num[9] = 0x30;
	
	if(n > 999999) {
		val = n / 1000000;
		num[10] = val + 0x30;
		n -= val * 1000000;
	} else num[10] = 0x30;
	
	if(n > 99999) {
		val = n / 100000;
		num[11] = val + 0x30;
		n -= val * 100000;
	} else num[11] = 0x30;
	
	if(n > 9999) {
		val = n / 10000;
		num[12] = val + 0x30;
		n -= val * 10000;
	} else num[12] = 0x30;
	
	if(n > 999) {
		val = n / 1000;
		num[13] = val + 0x30;
		n -= val * 1000;
	} else num[13] = 0x30;
	
	if(n > 99) {
		val = n / 100;
		num[14] = val + 0x30;
		n -= val * 100;
	} else num[14] = 0x30;
	
	if(n > 9) {
		val = n / 10;
		num[15] = val + 0x30;
		n -= val * 10;
	} else num[15] = 0x30;
	
	if(n > 0) {
		num[16] = n + 0x30;
	} else num[16] = 0x30;
	
	return num;
}

