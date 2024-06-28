/*
 * maze_path.h
 *
 *  Created on: 2017/10/20
 *      Author: 菫子
 */

#ifndef MAZE_PATH_H_
#define MAZE_PATH_H_

#include <machine.h>
#include "iodefine.h"
#include "r_init_clock.h"
#include "r_init_non_existent_port.h"
#include "r_init_stop_module.h"

#include "serial.h"
#include "pinsetting.h"
#include "init.h"
#include "wait_time.h"
#include "read_ad.h"
#include "motor.h"
#include "get_log.h"

#include "wall_control.h"
#include "maze_search.h"
#include "maze_map.h"
#include "mode.h"

extern unsigned short footmap[16][16];

//typedef enum {
//	turn_Left, //左
//	go_straight, //前
//	turn_Right//右
//} viola_turn;


void Goal_path(void);

void Path_largeturn_compress(void);
void Path_large_output(void);

void Make_path(void);
void Path_output(void);
void minimum_walk(void);

void Make_half_path(void);
void minimum_half_walk(volatile float);
void Large_turn_walk(volatile float,volatile float);

#endif /* MAZE_PATH_H_ */
