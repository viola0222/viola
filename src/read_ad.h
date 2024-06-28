/*
 * read_ad.h
 *
 *  Created on: 2017/05/22
 *      Author: 菫子
 */

#ifndef READ_AD_H_
#define READ_AD_H_

#include <machine.h>
#include "iodefine.h"
#include "r_init_clock.h"
#include "r_init_non_existent_port.h"
#include "r_init_stop_module.h"
#include "serial.h"
#include "init.h"


float Vattery(void);
float Encoder(void);
float EncoderR(void);
float EncoderL(void);
int Senser_L(void);
int Senser_CL(void);
int Senser_C(void);
int Senser_CR(void);
int Senser_R(void);
int Gyro(void);

#endif /* READ_AD_H_ */
