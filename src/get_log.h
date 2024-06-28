/*
 * get_log.h
 *
 *  Created on: 2017/09/18
 *      Author: 菫子
 */

#ifndef GET_LOG_H_
#define GET_LOG_H_

#include <machine.h>
#include "iodefine.h"
#include "r_init_clock.h"
#include "r_init_non_existent_port.h"
#include "r_init_stop_module.h"
#include "serial.h"
#include "init.h"
#include "pinsetting.h"
#include "wait_time.h"


extern volatile float LOGtori[1000][6];
extern volatile int count_time, Log_Flag;


int Log_memory(int, int, float*);
int Log_output(int, int);

#endif /* GET_LOG_H_ */
