/*
 * init.h
 *
 *  Created on: 2017/05/22
 *      Author: 菫子
 */

#ifndef INIT_H_
#define INIT_H_

#include <machine.h>
#include "iodefine.h"
#include "r_init_clock.h"
#include "r_init_non_existent_port.h"
#include "r_init_stop_module.h"
#include "serial.h"

void init_CMT(void);
void init_AD(void);
//void init_SCI(void);
void init_TPU(void);
void init_MTU(void);

#endif /* INIT_H_ */
