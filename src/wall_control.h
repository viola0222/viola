/*
 * wall_control.h
 *
 *  Created on: 2017/09/25
 *      Author: 菫子
 */

#ifndef WALL_CONTROL_H_
#define WALL_CONTROL_H_

#include <machine.h>
#include "iodefine.h"
#include "r_init_clock.h"
#include "r_init_non_existent_port.h"
#include "r_init_stop_module.h"
#include "read_ad.h"
#include "wait_time.h"
#include "maze_search.h"

//リファレンス値は壁から3cm離れたぐらいの時のセンサーの値が目安
//中心走るためのセンサー値
#define Centre_ref 2000
#define MRight_ref 650
#define MLeft_ref 700
#define MRight_real_ref 750
#define MLeft_real_ref 850
//柱見るセンサー君
#define Right_ref 30
#define Left_ref 30

//差分基準値　壁ないところに吸い込まれない用
/*壁制御ONorOFFの閾値*/
#define Centre_difference 3255
#define MRight_difference 12//20
#define MLeft_difference 12//20

//進むときに壁があるかどうかの基準閾値
#define Centre_thre 200
#define MRight_thre 300
#define MLeft_thre 250


//PID比例定数
/*前壁*/
#define KPFront_P_GAIN 0.01
#define KPFront_I_GAIN 0
/*横壁*/
#define KPwall_P_GAIN 0.21//未調整
#define KPwall_D_GAIN 0.002//未調整

#define Verocity_Wall 150

float Wall_Control(void);
float Front_Control(void);

#endif /* WALL_CONTROL_H_ */
