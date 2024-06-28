/*
 * wait_time.c
 *
 *  Created on: 2017/09/25
 *      Author: 菫子
 */
#include "wait_time.h"

extern volatile int count;

void wait_ms(volatile float time) {
	count = 0;
	while (count < time) {
	};
}

