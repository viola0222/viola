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
#include "maze_path.h"

#define KP_P_GAIN 0.5//1.12//1.1～1.17//0.175
#define KP_I_GAIN 0.0025//0.017//0.015～0.096だとでかい
#define KPomega_P_GAIN 0.32//0.32// 0.1//調整済み
#define KPomega_I_GAIN 0.002//0.002//0.0019//0.025//9/15 調整済み

#define KUKAKU 180 //１区画180mm

#define Gyro_Vdd 3.3//GyroのVdd
#define Gyro_dps 4095//Gyroの分解能
#define Gyro_Sensiviity_minus 0.6879//GyroのmV/dps
#define Gyro_Sensiviity_plus 0.688//ok
#define Gyro_Sensiviity_slalom_R 0.664//0.671
#define Gyro_Sensiviity_slalom_L 0.689//0.674

#define ON_Fail_Safe 100//100msたつとfailsafeのフラグを立てる

//速度は[mm/s]
//const volatile int hankei = 12;
//const volatile float gearhi = 3.33;

///XXX 大廻じゃないpathに既知壁いれて<-いれた

volatile int count, GyroFCount, GyroFlag, GyroSFlag, Speed_R, Speed_L, ENCODE_R,
		ENCODE_L, kasoku_time, count_time, log_time, failsafe_count;

//flag
volatile int DkFlag, Dk_omega_Flag, Dk_omega_minas_Flag, Log_Flag, Cho_Flag,
		Sake_Flag, Dk_END_Flag, Slalom_Flag, turn_R_Flag, turn_L_Flag,
		Sensor_L_Flag, Sensor_Centre_Flag, Sensor_R_Flag, FailSafe_Flag,
		Debug_Flag;

//verocity
volatile float Vg, Vr, Vl, Xg, Xg_target, guruguru_X, addGyro, Gyroheikin,
		Gyroomega, Gyrotheta, ENComega, ENCsita;

volatile float ErrorG_I, ErrorG_P, ErroromegaG_I, ErroromegaG_P, ControlG,
		ControlomegaG, ControlR, ControlL, ControlR_P, ControlL_P, ControlR_I,
		ControlL_I;

volatile float velo_R, velo_L, Vf, Vend, mGyro;

volatile float target_V, target_X, target_Accel, target_omega_V, target_omega_X,
		target_omega_Accel;

volatile int ErrorSenser_P, ErrorSenser_P_Before, ErrorSenser_D, ErrorFront_P,
		ErrorFront_P_Before, ErrorFront_D;
volatile float ControlSenserG, ControlFrontSenser;

volatile float LOGtori[1000][6] = { 0 }; //メモリ残量を気にすること.

float Log[6] = { 0 };
int SenL_Log[5] = { 0 };
int SenR_Log[5] = { 0 };
int dutyLog_R, dutyLog_L;

//volatile int Sen_C, Sen_CL, Sen_CR;

typedef struct {
	signed int val; //現在のセンサーの値
	signed int dif; //微分値(傾き)
	signed int threshold; //壁のあるなしの閾値
	signed int before; //前回のセンサー値
	signed int ref; //reference
} Sensor;

Sensor Centre, Left, Right, MRight, MLeft;
Sensor OFF_Centre, OFF_Left, OFF_Right, OFF_MRight, OFF_MLeft;

extern direction Past_compass, Now_compass;
extern volatile int Here_X, Here_Y, Comeback_Flag, Goal_Flag;
//extern unsigned short footmap[16][16];
extern int head_Queue, tail_Queue, head_path_Queue, tail_path_Queue;
extern int U_turn_Flag, U_turn_count, Circuit_Flag, Circuit_count,
		Path_memory_generate_Flag;
/*extern unsigned short sideX_wall[17][16]; //X方向の壁
 extern unsigned short sideY_wall[16][17]; //Y方向の壁
 */
//read_ad.c
//float Encoder();
//float EncoderR();
//float EncoderL();
//int Senser_C();
//void Hasiru(volatile int, volatile int);
//void Linear_motion(volatile float, volatile float, volatile float,
//		volatile float, volatile float);
//void Duty_omega(volatile float);
//int Gyro();read_ad.c
//void motor_off(void);
/*速度の単位は[mm/s]*/

void PID_calc(volatile float, volatile float);
void Gyro_calc(void);

void interrupt_cmt0(void) {
	volatile int i;

	PORT0.PODR.BIT.B5 = 0;			//センサーLED CL
	PORT4.PODR.BIT.B2 = 0;			//L
	PORT4.PODR.BIT.B3 = 0;			//R
	PORTE.PODR.BIT.B0 = 0;			//CR
	PORTE.PODR.BIT.B2 = 0;			//C

	OFF_Centre.val = Senser_C();
	OFF_MRight.val = Senser_CR();
	OFF_MLeft.val = Senser_CL();

//	Right.val = Senser_R();
//	Left.val = Senser_L();

//センサーLED C
	PORTE.PODR.BIT.B2 = 1;

	for (i = 0; i <= 500; i++)
		;

	Centre.val = Senser_C() - OFF_Centre.val;

	PORTE.PODR.BIT.B2 = 0;

//センサーLED CR
	PORTE.PODR.BIT.B0 = 1;

	for (i = 0; i <= 500; i++)
		;

	MRight.val = Senser_CR() - OFF_MRight.val;

	PORTE.PODR.BIT.B0 = 0;

	//センサーLED CL
	PORT0.PODR.BIT.B5 = 1;

	for (i = 0; i <= 500; i++)
		;

	MLeft.val = Senser_CL() - OFF_MLeft.val;

	PORTE.PODR.BIT.B5 = 0;

	if (Centre.val > 3590) {
		failsafe_count++;
		if (failsafe_count > ON_Fail_Safe) {
			FailSafe_Flag = 1;
		}
	} else {
		failsafe_count = 0;
	}

	Vr = EncoderR();
	Vl = EncoderL();
	Vg = (Vr + Vl) / 2;

	/*右が正, 左が負*/

	Xg = Xg + Vg * 0.001;

	target_V = target_V + target_Accel * 0.001;
	target_X = target_X + target_V * 0.001;

	target_omega_X = target_omega_X + target_omega_V * 0.001;
	target_omega_V = target_omega_V + target_omega_Accel * 0.001;

	Gyro_calc();

//	Log[0] = count_time;
//	Log[1] = target_omega_V;
//	Log[2] = target_V;
//	Log[3] = Gyroomega;
//	Log[4] = Vg;

	if (DkFlag == 1) {
//		if (count_time % 5 == 0) {

		MRight.before = SenR_Log[count_time % 5];
		MLeft.before = SenL_Log[count_time % 5];

		MRight.dif = MRight.before - MRight.val;
		MLeft.dif = MLeft.before - MLeft.val;

		if (MRight.dif < 0) {
			MRight.dif = -MRight.dif;
		}
		if (MLeft.dif < 0) {
			MLeft.dif = -MLeft.dif;
		}

		SenL_Log[count_time % 5] = MLeft.val;
		SenR_Log[count_time % 5] = MRight.val;

//		}
		if (Log_Flag == 1) {
			if (count_time % 2 == 0) {

				// get_logs(log_time, count_time, Gyroomega, Gyrotheta, target_omega_V,
				// 		target_omega_Accel, target_omega_X);

				// get_logs(log_time, count_time, MLeft.val, MLeft.dif, MRight.val, MRight.dif,
				// 		Centre.val);

				// get_logs(log_time, count_time, Vg, target_V, Xg, target_X, Gyrotheta);

				// get_logs(log_time, count_time, MLeft.val, MLeft.dif, MRight.val, Centre.val,
				// 		target_omega_V);

				// log_time++;
			}
		}
		PID_calc(target_V, 0);

	} else if (Dk_omega_Flag == 1) {
		if (Log_Flag == 1) {
			if (count_time % 2 == 0) {
				// get_logs(log_time, count_time, Gyroomega, target_omega_V, Gyrotheta, target_omega_X,
				// 		0.f);

				// get_logs(log_time, count_time, MLeft.val, Centre.dif, MRight.val, Gyrotheta,
				// 		0.f);

				log_time++;
			}
		}

		PID_calc(0, target_omega_V);

	} else if (Slalom_Flag == 1) {
		if (Log_Flag == 1) {
			if (count_time % 2 == 0) {
				// get_logs(log_time, count_time, Gyroomega, Gyrotheta, target_omega_V, target_omega_Accel,
				// 		target_omega_X);

				log_time++;
			}
		}
		PID_calc(target_V, target_omega_V);

	} else if (Sake_Flag == 1) {
		get_logs(log_time, count_time, ErrorG_P, ErrorG_I, ErroromegaG_P,
				ErroromegaG_I, 0.f);

		log_time++;

		PID_calc(0, 0);

	} else {

		motor_off();

	}

	count++;
	count_time++;

}

void Gyro_calc(void) {
	mGyro = (float) Gyro();

	if (turn_R_Flag == 1) {
		/*右スラローム*/
		Gyroomega = ((((mGyro - Gyroheikin) * Gyro_Vdd) / Gyro_dps) * 1000
				/ Gyro_Sensiviity_slalom_R); //Vdd=3.3V,AD dps=4096,(2000dps),Sensivity=0.67mV/dps,
	} else if (turn_L_Flag == 1) {
		/*左スラローム*/
		Gyroomega = ((((mGyro - Gyroheikin) * Gyro_Vdd) / Gyro_dps) * 1000
				/ Gyro_Sensiviity_slalom_L); //Vdd=3.3V,AD dps=4096,(2000dps),Sensivity=0.67mV/dps,
	} else if (Dk_omega_minas_Flag == 1) {
		/*右超新地*/
		Gyroomega = ((((mGyro - Gyroheikin) * Gyro_Vdd) / Gyro_dps) * 1000
				/ Gyro_Sensiviity_minus); //Vdd=3.3V,AD dps=4096,(2000dps),Sensivity=0.67mV/dps,
	} else {
		/*左超新地*/
		Gyroomega = ((((mGyro - Gyroheikin) * Gyro_Vdd) / Gyro_dps) * 1000
				/ Gyro_Sensiviity_plus); //Vdd=3.3V,AD dps=4096,(2000dps),Sensivity=0.67mV/dps,
	}

	Gyrotheta += Gyroomega / 1000;

	if (GyroFlag == 1) {
		GyroFCount++;
		addGyro += mGyro;

		if (GyroFCount == (2000 - 1)) {
			Gyroheikin = addGyro / 2000;
			Gyrotheta = 0;
			GyroFCount = 0;
			GyroFlag = 0;

		}
	}
}

/* motor.c*/
void PID_calc(volatile float tar_V, volatile float target_omegaV) {

//直進方向
	//壁制御
//	ControlFrontSenser = Front_Control();
	ControlFrontSenser = 0;
	tar_V += ControlFrontSenser;

	//前壁制御いれたので直進に追従させるぞ
	ErrorG_P = (tar_V - Vg);
	ErrorG_I += (tar_V - Vg);
	ControlG = KP_P_GAIN * ErrorG_P + KP_I_GAIN * ErrorG_I;

//壁制御
	ControlSenserG = Wall_Control();

//	ControlSenserG = 0;
//壁制御いれたので角度に追従させるんだぞ
	target_omegaV += ControlSenserG;

//角度方向
	ErroromegaG_P = (target_omegaV - Gyroomega);
	ErroromegaG_I += (target_omegaV - Gyroomega);
	ControlomegaG = KPomega_P_GAIN * ErroromegaG_P
			+ KPomega_I_GAIN * ErroromegaG_I;

	dutyLog_R = ControlG + ControlomegaG;
	dutyLog_L = ControlG - ControlomegaG;
	Hasiru(ControlG + ControlomegaG, ControlG - ControlomegaG);
}

void Ennkaigei(volatile int sec) {
	Sake_Flag = 1;
	wait_ms(sec);
	Sake_Flag = 0;
}

void main(void) {
//	volatile float velo, LEDL, LEDC, SENC;
	volatile int i;
	volatile float mode;
	/* ---- Disable maskable interrupts ---- */
	clrpsw_i();

	/* ---- Stopping the peripherals which start operations  ---- */
	R_INIT_StopModule();

	/* ---- Initialization of the non-existent ports ---- */
	R_INIT_NonExistentPort();

	/* ---- Initialization of the clock ---- */
	R_INIT_Clock();

	setpsw_i();

	initSCI();

	init_CMT();
	init_AD();
	init_TPU();
	init_MTU();
	pin_setting();

	Cho_Flag = 0;
	DkFlag = 0;
	Dk_omega_Flag = 0;
	Log_Flag = 0;
	Sake_Flag = 0;
	Dk_END_Flag = 0;
	Slalom_Flag = 0;
	turn_R_Flag = 0;
	turn_L_Flag = 0;
	U_turn_Flag = 0;
	head_Queue = 0;
	tail_Queue = 0;
	head_path_Queue = 0;
	tail_path_Queue = 0;
	U_turn_count = 0;
	Sensor_L_Flag = 0;
	Sensor_Centre_Flag = 0;
	Sensor_R_Flag = 0;
	Comeback_Flag = 0;
	Circuit_Flag = 0;
	Circuit_count = 0;
	Goal_Flag = 0;
	Path_memory_generate_Flag = 0;

//	ICUb.NMI-p410
	PORT3.PODR.BIT.B1 = 0;			//モーターを動かないようSTBY状態に

	PORT5.PODR.BIT.B5 = 0;			//IN2_L
	PORT5.PODR.BIT.B4 = 0;			//IN1_L
	PORTB.PODR.BIT.B7 = 0;			//IN2_R
	PORTB.PODR.BIT.B6 = 0;			//IN1_R

	wait_ms(500);
	Vattery();

//	while(1)
//	{
//		//１が右で2が左
//		myprintf("%5d,%5d\n", TPU1.TCNT, TPU2.TCNT);
//	}

	PORT0.PODR.BIT.B5 = 0;			//センサーLED CL
	PORT4.PODR.BIT.B2 = 0;			//L
	PORT4.PODR.BIT.B3 = 0;			//R
	PORTE.PODR.BIT.B0 = 0;			//CR
	PORTE.PODR.BIT.B2 = 0;			//C

//センサー値初期化
	Centre.val = 0;
	MRight.val = 0;
	MLeft.val = 0;
	Right.val = 0;
	Left.val = 0;
	OFF_Centre.val = 0;
	OFF_MRight.val = 0;
	OFF_MLeft.val = 0;
	OFF_Right.val = 0;
	OFF_Left.val = 0;

	Centre.dif = 0;
	MRight.dif = 0;
	MLeft.dif = 0;
	Right.dif = 0;
	Left.dif = 0;

//閾値

	MRight.threshold = MRight_thre;
	MLeft.threshold = MLeft_thre;
	MRight.ref = MRight_ref;
	MLeft.ref = MLeft_ref;

//	wait_ms(2000);			//置くのに揺らしてもこれで大丈夫！

	PORTA.PODR.BIT.B3 = 1;
	PORTA.PODR.BIT.B4 = 1;
	PORTA.PODR.BIT.B6 = 1;

//	myprintf(
//			"MLeft.val,OFF_MLeft.val, Centre.val,OFF_Centre.val, MRight.val,OFF_MRight.val\n");
//
//	while (1) {
//		myprintf("  %d,%d, %d,%d, %d,%d\n", MLeft.val, OFF_MLeft.val,
//				Centre.val, OFF_Centre.val, MRight.val, OFF_MRight.val);
////		myprintf("%d, %d\n", MLeft.val, OFF_MLeft.val);
//
//	}

//	while (1) {
//		myprintf("  %d,%d, %d,%d, %d,%d\n", MLeft.val, OFF_MLeft.val,
//				Centre.val, OFF_Centre.val, MRight.val, OFF_MRight.val);
//		wait_ms(10);
//	}

//	while(1){
//		mode=EncoderR();
//		myprintf("%f \n",mode);
//		wait_ms(500);
//	}

	while (1) {
		mode_choice();
	}

	while (1)
		;

	while (Centre.val < 1000)
		;			//真ん中センサーでスタートしてほしい

	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B6 = 0;

	GyroFlag = 0;
	Xg = 0;
	Vg = 0;
	Vr = 0;
	Vl = 0;
//Gyroの測定の初期化
	Gyroheikin = 0;
	Gyrotheta = 0;
	addGyro = 0;
	GyroFCount = 0;

	GyroFlag = 1;
	wait_ms(2000);			//Gyroの中心測定

//制御出力値の初期化
	ErrorG_P = 0;
	ErrorG_I = 0;
	ControlG = 0;

	ErroromegaG_P = 0;
	ErroromegaG_I = 0;
	ControlomegaG = 0;

	ErrorSenser_P = 0;
	ErrorSenser_D = 0;
	ControlSenserG = 0;

	ErrorFront_P = 0;
	ErrorFront_D = 0;
	ControlFrontSenser = 0;

//エンコーダー初期化
	TPU1.TCNT = 32768;
	TPU2.TCNT = 32768;

//位置の初期化
	target_Accel = 0;
	target_V = 0;
	target_X = 0;

	target_omega_Accel = 0;
	target_omega_V = 0;
	target_omega_X = 0;

	Past_compass = North;
	Now_compass = North;

	Here_X = Start_X;
	Here_Y = Start_Y;

//	set_wall(0, 1, East);
//	output_maze_cost();
//
//	while (1) {
//
//	}

	PORT3.PODR.BIT.B1 = 1;			//モーターのSTBYを解除

//	Initialize();
	Comeback_Flag = 0;
	Linear_motion(90, 5000, 500, 0, 500);
	while (Goal_Flag == 0) {
		Adachi_method();
	}

	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B6 = 0;

	Goal_Flag = 0;
	Comeback_Flag = 1;
	Linear_motion(90, 5000, 500, 0, 500);
	Im_here(Now_compass);
	Past_compass = Now_compass;
	while (Goal_Flag == 0) {
		Adachi_method();
	}
	while (1)
		;

//	Ultra_pivot_turn(360, 5000, 300, 0, 0);

//	Linear_motion(1260, 5000, 500, 0, 0);
//	Linear_motion(90, 10000, 500, 500, 0);
//	Circuit_Circuit();

//	Linear_motion(90, 5000, 500, 0, 500);
//	while (1) {
////		Left_hands();
////		Left_hands_Slalom();
//		Adachi_method();
////		Circuit_Flag = 1;
////		Left_hands_getwall();
//	}

//	Linear_motion(1260, 5000, 500, 0, 0);
//	Ultra_pivot_turn(180, 10000, 300, 0, 0);
//	for (i = 0; i < 10; i++) {
//		Ultra_pivot_turn(180, 10000, 300, 0, 0);
//	}
//	Linear_motion(720, 5000, 500, 0, 0);

//	Slalom(90, 10000, 400, 500, 1,10);
//	Linear_motion(90, 5000, 500, 0, 500);
//	Linear_motion(14, 5000, 500, 500, 500);
//	Slalom(90, -10000, -400, 500, 0,0);
//	Linear_motion(90, 5000, 500, 500, 0);
//	for (i = 0; i <= 8; i++) {
////		Slalom(90, -10000, -320, 400, 0);
//		Linear_motion(5, 5000, 500, 500, 500);
//		Slalom(90, 10000, 400, 500, 0, 0);
//		Linear_motion(10, 5000, 500, 500, 500);
//	}

//	Linear_motion(180, 5000, 500, 0, 500);
//	for (i = 0; i < 40; i++) {
//		Linear_motion(90, 5000, 500, 500, 500);
//		turn_L_Flag = 1;
//		Linear_motion(5, 5000, 500, 500, 500);
//		Slalom(90, 10000, 450, 500, 0, 0);
//		Linear_motion(10, 5000, 500, 500, 500);
//		turn_L_Flag = 0;
//		Linear_motion(90, 5000, 500, 500, 500);
//	}
//	Linear_motion(180, 5000, 500, 500, 0);

//	Slalom_Flag = 1;
//	Linear_motion(14, 5000, 500, 500, 500);
//	Slalom(90, -10000, -400, 500, 0, 0);
//
//	Linear_motion(2, 5000, 500, 500, 500);
//	Slalom(90, -10000, -400, 500, 0, 0);
//	Linear_motion(5, 5000, 500, 500, 500);
//
//	Linear_motion(90, 5000, 500, 500, 0);
//	Slalom_Flag = 0;
//	Linear_motion(15, 5000, 500, 500, 500);
//	Slalom(90, 10000, 450, 500, 0, 0);
//	Linear_motion(10, 5000, 500, 500, 500);

//	turn_R_Flag = 1;
//	Linear_motion(270, 5000, 500, 0, 500);
//	Linear_motion(2, 5000, 500, 500, 500);
//	Slalom(90, -10000, -400, 500, 0, 0);
//	Linear_motion(5, 5000, 500, 500, 500);
//	Linear_motion(270, 5000, 500, 500, 0);
//	turn_R_Flag = 0;

	/*スラローム調整　335じゃちょっと大きそう　230はちいさい　わかめ*/
	/*	Linear_motion(270, 5000, 400, 0, 400);
	 Slalom(90, -10000, -335, 400, 0);
	 Linear_motion(90, 5000, 400, target_V, 400);
	 while (1) {
	 Linear_motion(90, 5000, 400, target_V, 400);
	 Slalom(90, -10000, -335, 400, 0);
	 Linear_motion(90, 5000, 400, target_V, 400);
	 }
	 */

//	Ultra_pivot_turn(180,5000,300,0,0);
//	Ultra_pivot_turn(180, -5000, -300, 0, 0);
//	Linear_motion(270, 5000, 400, 0, 400);
//	Slalom(90, 5000, 300, 400, 5);
//	Linear_motion(270, 5000, 400, 400, 0);
//	while (1) {
//		myprintf("  %d,%d, %d,%d, %d,%d\n", MLeft.val, OFF_MLeft.val,
//				Centre.val, OFF_Centre.val, MRight.val, OFF_MRight.val);
//		//		myprintf("%d, %d\n", MLeft.val, OFF_MLeft.val);
//
//	}
//	Linear_motion(540, 5000, 400, 0, 0);
//	Ultra_pivot_turn(1800, -5000, -300, 0, 0);
//button push でログ取れるよ
	wait_ms(1000);
	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B4 = 1;
	PORTA.PODR.BIT.B6 = 0;
	wait_ms(1000);
	PORTA.PODR.BIT.B3 = 1;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B6 = 1;

	wait_ms(1000);
	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B4 = 1;
	PORTA.PODR.BIT.B6 = 0;
	wait_ms(1000);
	PORTA.PODR.BIT.B3 = 1;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B6 = 1;

	while (PORT3.PIDR.BIT.B5 == 0)
		;
	Log_output(log_time, 5);
	log_time++;
//	MAP_output();

	while (1) {

		wait_ms(100);

		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;

		/* LED3つのやつ
		 PORTA.PODR.BIT.B3 = 0;
		 PORTA.PODR.BIT.B4 = 0;
		 PORTA.PODR.BIT.B6 = 0;
		 */

	}
}
