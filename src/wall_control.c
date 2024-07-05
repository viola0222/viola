/*
 * wall_control.c
 *
 *  Created on: 2017/09/25
 *      Author: 菫子
 */
#include "wall_control.h"

typedef struct {
	signed int val; //現在のセンサーの値
	signed int dif; //微分値(傾き)
	signed int threshold; //壁のあるなしの閾値
	signed int before; //前回のセンサー値
	signed int ref; //reference
} Sensor;
extern volatile int Dk_omega_Flag, Dk_END_Flag, Slalom_Flag, U_turn_Flag,
		Debug_Flag;
//turn_R_Flag,turn_L_Flag;
extern Sensor Centre, MRight, MLeft;
extern volatile int ErrorSenser_P, ErrorSenser_P_Before, ErrorSenser_D,
		ErrorFront_P, ErrorFront_P_Before, ErrorFront_D;
extern volatile float ControlSenserG, ControlFrontSenser, Vg;

float Wall_Control(void) {

	if ((Dk_omega_Flag == 1) || (Dk_END_Flag == 1)
			|| (Centre.val >= Centre_difference) || (Slalom_Flag == 1)
			|| (Vg < Verocity_Wall) || (Debug_Flag == 1)) {
		//横壁制御入れない

		ControlSenserG = 0;
		return ControlSenserG;
	}
	//壁制御
	//吸い込まれ対策
	if (MLeft.dif > MLeft_difference) {
		MLeft.threshold = MLeft_ref + 20;
		MRight.ref = MRight_real_ref;
	} else {
		MLeft.threshold = MLeft_thre;
		MRight.ref = MRight_ref;
	}

	if (MRight.dif > MRight_difference) {
		MRight.threshold = MRight_ref + 20;
		MLeft.ref = MLeft_real_ref;
	} else {
		MRight.threshold = MRight_thre;
		MLeft.ref = MLeft_ref;
	}

//	if (turn_R_Flag == 1) {//右に曲がるとき
//		MLeft.ref += 30;
//	}
//	if (turn_L_Flag == 1) {//右に曲がるとき
//		MRight.ref += 30;
//	}

	if ((MRight.val >= MRight.threshold) && (MLeft.val >= MLeft.threshold)) {
		//両壁あるやで センサー値も下がってないやで

		ErrorSenser_P = (MRight.val - MRight.ref) - (MLeft.val - MLeft.ref);

	} else if ((MRight.val < MRight.threshold)
			&& (MLeft.val < MLeft.threshold)) { //両壁ないやで

		ErrorSenser_P = 0;

	} else if (MRight.val >= MRight.threshold) {

		ErrorSenser_P = 2 * (MRight.val - MRight.ref); // 右壁だけあるやで

	} else if (MLeft.val >= MLeft.threshold) {
		ErrorSenser_P = -2 * (MLeft.val - MLeft.ref); //左壁だけあるやで
	}

	ErrorSenser_D = ErrorSenser_P - ErrorSenser_P_Before;
	ErrorSenser_P_Before = ErrorSenser_P;

	ControlSenserG = KPwall_P_GAIN * ErrorSenser_P
			+ KPwall_D_GAIN * ErrorSenser_D;

	return ControlSenserG;
}

float Front_Control(void) {
	if ((U_turn_Flag == 1) && (Centre.val > Centre_difference)) {
		ErrorFront_P = Centre.val - Centre_ref;
		ErrorFront_D = ErrorFront_P - ErrorFront_P_Before;
		ErrorFront_P_Before = ErrorFront_P;

		ControlFrontSenser = KPFront_P_GAIN * ErrorFront_P
				+ KPFront_I_GAIN * ErrorFront_D;
	} else {
		ControlFrontSenser = 0;
	}

	return ControlFrontSenser;
}
