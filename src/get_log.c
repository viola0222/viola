/*
 * get_log.c
 *
 *  Created on: 2017/09/18
 *      Author: 菫子
 */
#include "get_log.h"

int Log_memory(int line, int elements, float *a) {

	volatile int j;
	if (elements > 6 || line > 1000) { //要素数 1000

	} else {
		for (j = 0; j < elements; j++) {
			LOGtori[line][j] = *(a + j);
		}
	}
	return 0;
}

int Log_output(int line, int elements) {

	volatile int i, j;
	if (elements > 6 || line > 1000) { //要素数7個以上, 1000行以上はエラー処理
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


void get_logs(int log_time, float data1, float data2, float data3, float data4, float data5, float data6){
	float Log[6] = { 0 };
	Log[0] = data1;
	Log[1] = data2;
	Log[2] = data3;
	Log[3] = data4;
	Log[4] = data5;
	Log[5] = data6;

	Log_memory(log_time, 6, Log)
}
