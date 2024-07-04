/*
 * mode.c
 *
 *  Created on: 2017/10/13
 *      Author: 菫子
 */
#include "mode.h"

typedef enum {
	Adachi, //足立法
	Adachi_comeback, //名前思いつかん
	Left_hand, //左手法
	Fast, //最短
	Map_export, //マップ出力
	Debug, //デバッグ用
	Circuit, //サーキット
	Speaker //おもちゃ
} mode_select;

typedef struct {
	signed int val; //現在のセンサーの値
	signed int dif; //微分値(傾き)
	signed int threshold; //壁のあるなしの閾値
	signed int before; //前回のセンサー値
	signed int ref; //reference
} Sensor;

extern Sensor Centre, Left, Right, MRight, MLeft;
extern Sensor OFF_Centre, OFF_Left, OFF_Right, OFF_MRight, OFF_MLeft;
extern direction Past_compass, Now_compass;
char mode_rabit;
mode_select pizza;

///XXX ターン調整
///XXX 前壁補正のいい塩梅ぐあいと進みすぎた時の処理を入れる.
///XXX タイヤ径合わせて
///XXX　まずFF入れる.
///XXX 完了 台形加速のデバッグ. 多分超新地とかスラロームにも問題があるので見直す.
///XXX フラグ管理が甘いのでstruct　or boolで宣言しなおす
///XXX モード選択のデバック用の強化
///XXX 完了 スラロームの調整とスラローム右左折用の関数の作成(足立法とかのが見づらいのと東北の悪夢を繰り返さないため)
///XXX スピーカー鳴らす
///XXX 前壁制御と壁切れを入れる
///XXX log取れるかの確認

void Initialize(void) {
	while (Centre.val < 1000)
		;			//真ん中センサーでスタートしてほしい

	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B6 = 0;

	FailSafe_Flag = 0;
	Goal_Flag = 0;
	GyroFlag = 0;
	Debug_Flag = 0;
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
	ErrorSenser_P_Before = 0;
	ErrorSenser_D = 0;

	ErrorFront_P = 0;
	ErrorFront_P_Before = 0;
	ErrorFront_D = 0;

	ControlSenserG = 0;
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

	PORT3.PODR.BIT.B1 = 1;			//モーターのSTBYを解除

}

void Command_input(void) {
//	mode_select pizza;
	float mode;
	FailSafe_Flag = 0;

	wait_ms(500);

	while (PORT3.PIDR.BIT.B5 == 0) {
		mode = Vr;
//		myprintf("%f \n", mode);
		wait_ms(80);
		if (mode >= 250) {
			mode_rabit++;
		} else if (mode <= -250) {
			mode_rabit--;
		}

		if (mode_rabit < 1) {
			mode_rabit = 8;
		}

		if (mode_rabit > 9) {
			mode_rabit = 1;
		}

		if (mode_rabit == 1) {
			PORTA.PODR.BIT.B3 = 0;
			PORTA.PODR.BIT.B4 = 0;
			PORTA.PODR.BIT.B6 = 1;

		} else if (mode_rabit == 2) {
			PORTA.PODR.BIT.B3 = 0;
			PORTA.PODR.BIT.B4 = 1;
			PORTA.PODR.BIT.B6 = 0;

		} else if (mode_rabit == 3) {
			PORTA.PODR.BIT.B3 = 0;
			PORTA.PODR.BIT.B4 = 1;
			PORTA.PODR.BIT.B6 = 1;

		} else if (mode_rabit == 4) {
			PORTA.PODR.BIT.B3 = 1;
			PORTA.PODR.BIT.B4 = 0;
			PORTA.PODR.BIT.B6 = 0;

		} else if (mode_rabit == 5) {
			PORTA.PODR.BIT.B3 = 1;
			PORTA.PODR.BIT.B4 = 0;
			PORTA.PODR.BIT.B6 = 1;

		} else if (mode_rabit == 6) {
			PORTA.PODR.BIT.B3 = 1;
			PORTA.PODR.BIT.B4 = 1;
			PORTA.PODR.BIT.B6 = 0;

		} else if (mode_rabit == 7) {
			PORTA.PODR.BIT.B3 = 1;
			PORTA.PODR.BIT.B4 = 1;
			PORTA.PODR.BIT.B6 = 1;

		} else if (mode_rabit == 8) {
			PORTA.PODR.BIT.B3 = 1;
			PORTA.PODR.BIT.B4 = 0;
			PORTA.PODR.BIT.B6 = 1;
		}
	}

	if (mode_rabit == 1) {
		pizza = Adachi;

	} else if (mode_rabit == 2) {
		pizza = Left_hand;

	} else if (mode_rabit == 3) {

		pizza = Adachi_comeback;

	} else if (mode_rabit == 4) {

		pizza = Map_export;

	} else if (mode_rabit == 5) {
		pizza = Fast;

	} else if (mode_rabit == 6) {
		pizza = Circuit;

	} else if (mode_rabit == 7) {
		pizza = Debug;

	} else if (mode_rabit == 8) {
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;

		wait_ms(500);

		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;

		wait_ms(500);

		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 1;

		pizza = Speaker;
	}
//	return pizza;
}

void mode_choice(void) {

	int tape;
	mode_rabit = 0;
	pizza = Adachi;
//	myprintf("select mode\n");
	PORTA.PODR.BIT.B3 = 1;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B6 = 0;
	wait_ms(100);
	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B4 = 1;
	PORTA.PODR.BIT.B6 = 0;
	wait_ms(100);
	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B6 = 1;
	wait_ms(100);
	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B4 = 1;
	PORTA.PODR.BIT.B6 = 0;
	wait_ms(100);
	PORTA.PODR.BIT.B3 = 1;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B6 = 0;
	wait_ms(100);
	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B6 = 0;
	wait_ms(100);
	Command_input();
	wait_ms(1000);

	switch (pizza) {
	case Adachi:
		Initialize();
		Linear_motion(90, 5000, 500, 0, 500);
		Comeback_Flag = 0;
		while (Goal_Flag == 0) {
			Adachi_method();
			if (FailSafe_Flag == 1) {
				break;
			}
		}
		while (PORT3.PIDR.BIT.B5 == 0)
			;
		output_maze_cost();
		output_Path_maze_cost();
//		Make_half_path();
//		Path_largeturn_compress();
		Path_Generater();
		Path_print();

		break;

	case Adachi_comeback:
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(200);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(200);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 1;
		wait_ms(200);

		Initialize();
		Comeback_Flag = 0;
		Linear_motion(90, 5000, 500, 0, 500);
		while (Goal_Flag == 0) {
			Adachi_method();
			if (FailSafe_Flag == 1) {
				break;
			}
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
			if (FailSafe_Flag == 1) {
				break;
			}
		}

		while (PORT3.PIDR.BIT.B5 == 0)
			;
		Goal_Flag = 0;
		Path_Generater();
		Path_print();
//		Make_half_path();
//		Path_largeturn_compress();

		break;

	case Left_hand:

		Path_end_Flag = 0;

//		while (PORT3.PIDR.BIT.B5 == 0)
//			;
//		Path_output();
//		Path_large_output();

		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 1;
		wait_ms(200);
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 1;
		wait_ms(100);

//		while (PORT3.PIDR.BIT.B5 == 0)
//			;
		Comeback_Flag = 0;
		Initialize();
		Linear_motion(90, 5000, 500, 0, 500);
		while (Goal_Flag == 0) {
 			Left_hands_getwall();
		}
// //		myprintf("Now Lefthand.\n");
		Path_Generater();
		break;

	case Fast:

		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 1;
		wait_ms(100);

		Initialize();
		Comeback_Flag = 0;
		minimum_path_walk(1500);
		break;

	case Map_export:
		while (PORT3.PIDR.BIT.B5 == 0)
			;
		output_maze_cost();
		output_Path_maze_cost();
		Path_Generater();
		Path_print();
		break;

	case Debug:
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(200);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(200);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 1;
		wait_ms(200);

//		while (1) {
//			myprintf("  %d,%d, %d,%d, %d,%d\n", MLeft.val, OFF_MLeft.val,
//					Centre.val, OFF_Centre.val, MRight.val, OFF_MRight.val);
//			//		myprintf("%d, %d\n", MLeft.val, OFF_MLeft.val);
//		}

		Initialize();
		Debug_Flag = 1;
//		Log_Flag = 1;

		while (PORT3.PIDR.BIT.B5 == 0) {
			PID_calc(0, 0);
		}

//		Linear_motion(720, 10000, 800, 0, 0);
//		Linear_motion(2520, 5000, 2000, 0, 1000);
//		Linear_motion(270, 5000, 500, 0, 500);
//		Linear_motion(180, 5000, 800, 0, 800);
//		Left_turn();
//		Linear_motion(180, 5000, 800, 800, 0);
//		Linear_motion(720, 5000, 2000, 1000, 0);
//		Linear_motion(270, 5000, 500, 500, 0);

//		Linear_motion(270, 10000, 500, 0, 500);
//		Left_turn();
//		Linear_motion(270, 10000, 500, 500, 0);
//		while (1) {
//			myprintf("%d\n", Centre.val);
//		}

//		Linear_motion(180, 5000, 500, 0, 500);
//		for (tape = 0; tape < 8; tape++) {
//			Linear_motion(90, 5000, 500, 500, 500);
//			Right_turn();
//			Linear_motion(90, 5000, 500, 500, 500);
//		}
//		Linear_motion(180, 5000, 500, 500, 0);
		Debug_Flag = 0;
//
//		Linear_motion(90, 5000, 500, 500, 0);

//		Ultra_pivot_turn(1800, 5000, 500, 0, 0);
//		for (tape = 0; tape < 10; tape++) {
//			Ultra_pivot_turn(180, 5000, 500, 500, 500);
////			Ultra_pivot_turn(180, -5000, -500, 0, 0);
//		}
//		Ultra_pivot_turn(180, 5000, 500, 500, 0);
//		Linear_motion(540, 5000, 500, 0, 0);

		Log_Flag = 0;
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 1;
		wait_ms(100);

		while (PORT3.PIDR.BIT.B5 == 0)
			;
		Log_output(log_time, 6);
		Log_Flag = 0;
		break;

	case Circuit:

		Path_end_Flag = 0;

//		while (PORT3.PIDR.BIT.B5 == 0)
//			;
//		Path_output();
//		Path_large_output();

		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 1;
		wait_ms(200);
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 1;
		wait_ms(100);

//		while (PORT3.PIDR.BIT.B5 == 0)
//			;

		Initialize();
		Comeback_Flag = 0;
		minimum_half_walk(1200);
		break;

	case Speaker:

		Path_end_Flag = 0;

//		while (PORT3.PIDR.BIT.B5 == 0)
//			;
//		Path_output();
//		Path_large_output();

		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 1;
		wait_ms(200);
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(100);
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 1;
		wait_ms(100);

//		while (PORT3.PIDR.BIT.B5 == 0)
//			;

		Initialize();
		Comeback_Flag = 0;
		minimum_half_walk(1500);
		break;
	}

	wait_ms(1000);

	while (PORT3.PIDR.BIT.B5 == 0)
		;
	wait_ms(1000);
}
