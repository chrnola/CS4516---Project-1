#include "Utils.h"

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
