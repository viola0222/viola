/*
 * maze_search.h
 *
 *  Created on: 2017/09/25
 *      Author: 菫子
 */

#ifndef MAZE_SEARCH_H_
#define MAZE_SEARCH_H_

#include "wall_control.h"
#include "motor.h"
#include "maze_map.h"
extern volatile int Sensor_L_Flag, Sensor_Centre_Flag, Sensor_R_Flag;

void U_turn(void);
void Right_turn(void);
void Right_large_turn(int);
void Right_large_U_turn(int);
void Left_turn(void);
void Left_large_turn(int);
void Left_large_U_turn(int);
void Circuit_Circuit(void);
void Left_hands(void);
void Left_hands_Slalom(void);
void Adachi_method(void);
void Left_hands_getwall(void);
#endif /* MAZE_SEARCH_H_ */
