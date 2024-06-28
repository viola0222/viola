/*
 * motor.h
 *
 *  Created on: 2017/05/18
 *      Author: 菫子
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <machine.h>
#include "iodefine.h"
#include "r_init_clock.h"
#include "r_init_non_existent_port.h"
#include "r_init_stop_module.h"
#include "serial.h"
#include "init.h"
#include "pinsetting.h"
#include "read_ad.h"
#include "wait_time.h"

void Error_Reset(void);
float abs(float);
void motor_off(void);
void Hasiru(volatile int, volatile int);

int Linear_motion(volatile float, volatile float, volatile float,
		volatile float, volatile float);
void Ultra_pivot_turn(volatile float, volatile float, volatile float,
		volatile float, volatile float);
void Ultra_pivot_turn(volatile float, volatile float, volatile float,
		volatile float, volatile float);
void Slalom(volatile float, volatile float, volatile float, volatile float);
void Ultra_pivot_Slalom(volatile float, volatile float, volatile float,
		volatile float);

#endif /* MOTOR_H_ */
