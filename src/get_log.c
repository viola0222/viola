/*
 * get_log.c
 *
 *  Created on: 2017/09/18
 *      Author: 菫子
 */
#include "get_log.h"

int Log_memory(int line, int elements, float *a) {

	volatile int j;
	if (elements > 6 || line > 2000) { //要素数2000

	} else {
		for (j = 0; j < elements; j++) {
			LOGtori[line][j] = *(a + j);
		}
	}
	return 0;
}

int Log_output(int line, int elements) {

	volatile int i, j;
	if (elements > 6 || line > 2000) { //要素数7個以上, 2000行以上はエラー処理
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 1;
		wait_ms(1000);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(1000);
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 1;
		wait_ms(1000);
		return 0;
	}
	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B4 = 1;
	PORTA.PODR.BIT.B6 = 0;

	for (j = 0; j < line; j++) {
		for (i = 0; i < elements; i++) {
			myprintf("%.3f,", LOGtori[j][i]);
			PORTA.PODR.BIT.B4 = 1;
		}
		myprintf("\n");
		PORTA.PODR.BIT.B4 = 0;
	}
	return 0;
}

