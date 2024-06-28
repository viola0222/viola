/*
 * wait_time.h
 *
 *  Created on: 2017/09/25
 *      Author: 菫子
 */

#ifndef WAIT_TIME_H_
#define WAIT_TIME_H_

#include <machine.h>
#include "iodefine.h"
#include "r_init_clock.h"
#include "r_init_non_existent_port.h"
#include "r_init_stop_module.h"

#include "serial.h"
#include "pinsetting.h"
#include "init.h"

#include "read_ad.h"

void wait_ms(volatile float time);

#endif /* WAIT_TIME_H_ */
