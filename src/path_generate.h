/*
 * path_generate.h
 *
 *  Created on: 2017/12/22
 *      Author: 菫子
 */

#ifndef PATH_GENERATE_H_
#define PATH_GENERATE_H_

#include "maze_map.h"

typedef enum {
	turn_Left, //左
	go_straight, //前
	turn_Right, //右
	goal_in
} viola_move;
extern unsigned short footmap[16][16];
extern unsigned short Path_footmap[16][16];
extern volatile float target_V;

int watch_mapcost(void);
void Path_at_here(direction);
void Path_Generater(void);
void minimum_path_walk(volatile float);
void Path_print(void);

#endif /* PATH_GENERATE_H_ */
