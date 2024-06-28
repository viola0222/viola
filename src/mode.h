/*
 * mode.h
 *
 *  Created on: 2017/10/13
 *      Author: 菫子
 */

#ifndef MODE_H_
#define MODE_H_

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
#include "maze_path.h"
#include "path_generate.h"

//flag
extern int Path_end_Flag;
extern volatile int DkFlag, Dk_omega_Flag, Dk_omega_minas_Flag, Log_Flag,
		Cho_Flag, Sake_Flag, Dk_END_Flag, Slalom_Flag, turn_R_Flag, turn_L_Flag,
		FailSafe_Flag, Debug_Flag;

//verocity
extern volatile float Vg, Vr, Vl, Xg, Xg_target, guruguru_X, addGyro,
		Gyroheikin, Gyroomega, Gyrosita, ENComega, ENCsita;

extern volatile float ErrorG_I, ErrorG_P, ErroromegaG_I, ErroromegaG_P,
		ControlG, ControlomegaG, ControlR, ControlL, ControlR_P, ControlL_P,
		ControlR_I, ControlL_I;

extern volatile float velo_R, velo_L, Vf, Vend, mGyro;

extern volatile float target_V, target_X, target_Accel, target_omega_V,
		target_omega_X, target_omega_Accel;

extern volatile int ErrorSenser_P, ErrorSenser_P_Before, ErrorSenser_D,
		ErrorFront_P, ErrorFront_P_Before, ErrorFront_D;
;
extern volatile float ControlSenserG, ControlFrontSenser;

extern volatile int Here_X, Here_Y, Comeback_Flag, Goal_Flag;
;
extern volatile int count, GyroFCount, GyroFlag, GyroSFlag, Speed_R, Speed_L,
		ENCODE_R, ENCODE_L, kasoku_time, count_time, log_time;

void Initialize(void);
void Initialize_comeback(void);
void mode_choice(void);

#endif /* MODE_H_ */
