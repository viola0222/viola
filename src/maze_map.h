/*
 * maze_map.h
 *
 *  Created on: 2017/09/27
 *      Author: 菫子
 */

#ifndef MAZE_MAP_H_
#define MAZE_MAP_H_

#include "iodefine.h"
#include "motor.h"
#include "wait_time.h"
#include "wall_control.h"
#include "maze_path.h"
#include "path_generate.h"

#define Start_X 0
#define Start_Y 1

#define Goal_X 1
#define Goal_Y 0

extern volatile float target_V;
extern volatile int Sensor_L_Flag, Sensor_Centre_Flag, Sensor_R_Flag;

typedef enum {
	North, //北
	East, //東 →
	South, //南
	West //西 ←
} direction;

typedef enum {
	violaLeft, //左
	violaFront, //前
	violaRight, //右
	violaBehind //後ろ
} viola_looking;

int get_looking_wall(int, int, direction);

void Goal(void);
int Where_I_am(void);
void Im_here(direction);
void FIFO_Queue_push(int, int, int);
void FIFO_Queue_pop(void);
void MAPCost_Memory(void);
short get_footmap(int, int, direction);
short footmap_distinction(direction, int);
void Path_MAP(void);
void set_wall(int, int, direction);
int get_wall(int, int, direction);
void set_know_wall(int, int, direction);
int get_know_wall(int, int, direction);
direction God_direction(direction, viola_looking);
void wall_distinction(direction);
void output_maze(void);
void output_maze_cost(void);
void output_Path_maze_cost(void);
void MAP_WALL_REKO(direction);
void MAP_output_REKO(void);

#endif /* MAZE_MAP_H_ */
