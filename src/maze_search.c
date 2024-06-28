/*
 * maze_search.c
 *
 *  Created on: 2017/09/25
 *      Author: 菫子
 */
#include "maze_search.h"

#define Maekabe 455//スラローム中の前壁補正の閾値

typedef struct {
	signed int val; //現在のセンサーの値
	signed int dif; //微分値(傾き)
	signed int threshold; //壁のあるなしの閾値
	signed int before; //前回のセンサー値
	signed int ref; //reference
} Sensor;

extern volatile int turn_R_Flag, turn_L_Flag;
extern Sensor Centre, MRight, MLeft;
extern volatile float target_V;
extern unsigned short footmap[16][16];
extern volatile int Here_X, Here_Y, Goal_Flag;
int U_turn_Flag, U_turn_count, Circuit_Flag, Circuit_count;

direction Past_compass, Now_compass;
///XXX aaa
///todo pa

void U_turn(void) {
	if (Past_compass == North) { //北向きからUターンしたよ
		Now_compass = South;
	} else if (Past_compass == East) { //東向きからUターンしたよ
		Now_compass = West;
	} else if (Past_compass == South) { //南向きからUターンしたよ
		Now_compass = North;
	} else if (Past_compass == West) { //西向きからUターンしたよ
		Now_compass = East;
	}

	if (U_turn_Flag != 0) {
		U_turn_count++;
	} else {
		U_turn_count = 0;
	}
	/*前壁補正*/
	if (Centre.val >= Maekabe) {

	} else {
		while (Centre.val < Maekabe)
			;
	}

//	Linear_motion(30, 5000, 500, target_V, 300);
//	while (Centre.val <= 3340) {
//		Linear_motion(5, 5000, 500, 300, 300);
//	}
//	Linear_motion(5, 5000, 300, 300, 0);
	/*前壁補正終わり*/

	Linear_motion(90, 5000, 500, target_V, 0);
	Ultra_pivot_turn(180, 5000, 500, 0, 0);
	Linear_motion(90, 5000, 500, 0, 500);

	if (U_turn_count > 3) {
		Goal();
	}
	U_turn_Flag = 1;
}

void Right_turn(void) {
	/*右小回り90°*/
	if (Centre.val > Centre_thre) {
		if (Centre.val >= Maekabe) {

		} else {
			while (Centre.val < Maekabe)
				;
		}
	}
	Linear_motion(13, 5000, 500, 500, 500);
	turn_R_Flag = 1;
	Ultra_pivot_Slalom(90, -10000, -450, 500);
	turn_R_Flag = 0;
	Linear_motion(14, 5000, 500, 500, 500);

}

void Right_large_turn(int velo) {
	/*右大廻90°*/
	turn_R_Flag = 1;

	if (velo == 800) {

		Linear_motion(90, 5000, 800, 800, 800);
		Ultra_pivot_Slalom(90, -10000, -680, 800); //-630 べしパラメータ
		Linear_motion(90, 5000, 800, 800, 800);

	} else if (velo == 1000) {

		Linear_motion(53, 5000, 1000, 1000, 1000);
		Ultra_pivot_Slalom(89, -10000, -920, 1000);
		Linear_motion(90, 5000, 1000, 1000, 1000);

	}
	turn_R_Flag = 0;
}

void Right_large_U_turn(int velo) {
	/*右大廻180°*/
	turn_R_Flag = 1;

	if (velo == 800) {
		Linear_motion(30, 5000, 800, 800, 800);
		Ultra_pivot_Slalom(180, -5000, -550, 800);
		Linear_motion(47, 5000, 800, 800, 800);
	} else if (velo == 1000) {
		Linear_motion(10, 5000, 1000, 1000, 1000);
		Ultra_pivot_Slalom(180, -5000, -700, 1000);
		Linear_motion(45, 5000, 1000, 1000, 1000);
	}

	turn_R_Flag = 0;
}

void Left_turn(void) {
	/*左小廻90°*/
	if (Centre.val > Centre_thre) {
		if (Centre.val >= Maekabe) {

		} else {
			while (Centre.val < Maekabe)
				;
		}
	}

	Linear_motion(9, 5000, 500, 500, 500);
	turn_L_Flag = 1;
	Ultra_pivot_Slalom(90, 10000, 450, 500);
	turn_L_Flag = 0;
	Linear_motion(17, 5000, 500, 500, 500);

}

void Left_large_turn(int velo) {
	/*左大廻90°*/

	turn_L_Flag = 1;

	if (velo == 800) {

		Linear_motion(88, 5000, 800, 800, 800);
		Ultra_pivot_Slalom(90, 10000, 680, 800);
		Linear_motion(98, 5000, 800, 800, 800);

	} else if (velo == 1000) {

		Linear_motion(70, 5000, 1000, 1000, 1000);
		Ultra_pivot_Slalom(89, 10000, 920, 1000);
		Linear_motion(92, 5000, 1000, 1000, 1000);

	}

	turn_L_Flag = 0;
}

void Left_large_U_turn(int velo) {
	/*左大廻180°*/
	turn_L_Flag = 1;

	if (velo == 800) {

		Linear_motion(37, 5000, 800, 800, 800);
		Ultra_pivot_Slalom(180, 5000, 540, 800);
		Linear_motion(48, 5000, 800, 800, 800);

	} else if (velo == 1000) {

		Linear_motion(15, 5000, 1000, 1000, 1000);
		Ultra_pivot_Slalom(180, 5000, 690, 1000);
		Linear_motion(55, 5000, 1000, 1000, 1000);
	}
	turn_L_Flag = 0;
}

void Circuit_Circuit(void) {
	int i;

	for (i = 0; i < 8; i++) {
		Linear_motion(2520, 10000, 1000, 500, 500);
		/*右折*/
		turn_R_Flag = 1;
		Linear_motion(2, 5000, 500, 500, 500);
		Slalom(90, -10000, -400, 500);
		Linear_motion(5, 5000, 500, 500, 500);
		turn_R_Flag = 0;
	}
	Goal();
}

void Left_hands_Slalom(void) {
	//なんてなめらかー
	MAPCost_Memory();
	if (MLeft.val < MLeft_thre) {
		//左壁isない
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		//		wait_ms(1000);

		if (Past_compass == North) { //北向きから左折したよ
			Now_compass = West;
		} else if (Past_compass == East) { //東向きから左折したよ
			Now_compass = North;
		} else if (Past_compass == South) { //南向きから左折したよ
			Now_compass = East;
		} else if (Past_compass == West) { //西向きから左折したよ
			Now_compass = South;
		}

		turn_L_Flag = 1;
		Linear_motion(24, 5000, 500, 500, 500);
		Slalom(90, 10000, 450, 500);
		Linear_motion(13, 5000, 500, 500, 500);
		turn_L_Flag = 0;

//		Linear_motion(90, 5000, 400, target_V, 0);
//		Ultra_pivot_turn(90, 5000, 300, 0, 0);
//		Linear_motion(90, 5000, 400, 0, 300);
	} else if (Centre.val < Centre_thre) {
		//左壁あるし前壁ないよ→直進だオラ
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 1;

		Linear_motion(180, 5000, 500, target_V, 500);

	} else if (MRight.val < MRight_thre) {
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;
		//右壁だけないよ→右に曲がるぞ

		if (Past_compass == North) { //北向きから右折したよ
			Now_compass = East;
		} else if (Past_compass == East) { //東向きから右折したよ
			Now_compass = South;
		} else if (Past_compass == South) { //南向きから右折したよ
			Now_compass = West;
		} else if (Past_compass == West) { //西向きから右折したよ
			Now_compass = North;
		}

//		Linear_motion(12, 5000, 500, 500, 500);
//		Slalom(90, -10000, -400, 500, 0, 0);
//		Linear_motion(10, 5000, 500, 500, 500);

		/*	Slalom(90, -10000, -320, 400, 0,0);
		 Linear_motion(10, 5000, 500, 500, 500);
		 */
		turn_R_Flag = 1;
		Linear_motion(2, 5000, 500, 500, 500);
		Slalom(90, -10000, -400, 500);
		Linear_motion(5, 5000, 500, 500, 500);
		turn_R_Flag = 0;
		/*こっちは超新地*/
//		Linear_motion(90, 5000, 400, target_V, 0);
//		Ultra_pivot_turn(90, -5000, -300, 0, 0);
//		Linear_motion(90, 5000, 300, 0, 300);
	} else {
		//行き止まりやんけ(そっとUターン)
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;

		U_turn();
	}
	Im_here(Now_compass);
	wall_distinction(Now_compass);
	Past_compass = Now_compass;
}

void Left_hands(void) { //かくかくなほう
	if (MLeft.val < MLeft_thre) {
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		//左壁isない
//		wait_ms(1000);
		Linear_motion(90, 5000, 300, 0, 0);
		Ultra_pivot_turn(90, 5000, 300, 0, 0);
		Linear_motion(90, 5000, 300, 0, 0);
		if (Past_compass == North) { //北向きから左折したよ
			Now_compass = West;
		} else if (Past_compass == East) { //東向きから左折したよ
			Now_compass = North;
		} else if (Past_compass == South) { //南向きから左折したよ
			Now_compass = East;
		} else if (Past_compass == West) { //西向きから左折したよ
			Now_compass = South;
		}
		U_turn_Flag = 0;
	} else if (Centre.val < Centre_thre) {
		//左壁あるし前壁ないよ→直進だオラ
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 1;
//		wait_ms(3000);
		Linear_motion(180, 5000, 300, 0, 0);
		U_turn_Flag = 0;
	} else if (MRight.val < MRight_thre) {
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 0;
//		wait_ms(1000);
		//右壁だけないよ→右に曲がるぞ
		Linear_motion(90, 5000, 300, 0, 0);
		Ultra_pivot_turn(90, -5000, -300, 0, 0);
		Linear_motion(90, 5000, 300, 0, 0);
		if (Past_compass == North) { //北向きから右折したよ
			Now_compass = East;
		} else if (Past_compass == East) { //東向きから右折したよ
			Now_compass = South;
		} else if (Past_compass == South) { //南向きから右折したよ
			Now_compass = West;
		} else if (Past_compass == West) { //西向きから右折したよ
			Now_compass = North;
		}
		U_turn_Flag = 0;
	} else {
		//行き止まりやんけ(そっとUターン)
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
//		wait_ms(1000);
		Linear_motion(90, 5000, 300, 0, 0);
		Ultra_pivot_turn(180, 5000, 300, 0, 0);
		Linear_motion(90, 5000, 300, 0, 0);

		if (Past_compass == North) { //北向きからUターンしたよ
			Now_compass = South;
		} else if (Past_compass == East) { //東向きからUターンしたよ
			Now_compass = West;
		} else if (Past_compass == South) { //南向きからUターンしたよ
			Now_compass = North;
		} else if (Past_compass == West) { //西向きからUターンしたよ
			Now_compass = East;
		}
	}
	Im_here(Now_compass);
	wall_distinction(Now_compass);
	Past_compass = Now_compass;
}

void Adachi_method(void) {
	//なんてなめらかー
	int tomato;
	wall_distinction(Now_compass);
	set_know_wall(Here_X, Here_Y, God_direction(Now_compass, violaFront));
	set_know_wall(Here_X, Here_Y, God_direction(Now_compass, violaRight));
	set_know_wall(Here_X, Here_Y, God_direction(Now_compass, violaLeft));
	set_know_wall(Here_X, Here_Y, God_direction(Now_compass, violaBehind));

	MAPCost_Memory();
//	Path_MAP();
	tomato = Where_I_am();

	if (tomato == 1) {
		Goal_Flag = 1;
		return;
	}
	if ((get_wall(Here_X, Here_Y, God_direction(Now_compass, violaFront)) == 0)
			&& (footmap[Here_X][Here_Y]
					> footmap_distinction(Now_compass, violaFront))) {
		/*前壁ないよ→直進だオラ*/
		U_turn_Flag = 0;

		Linear_motion(180, 5000, 500, target_V, 500);

	} else if ((get_wall(Here_X, Here_Y, God_direction(Now_compass, violaRight))
			== 0)
			&& (footmap[Here_X][Here_Y]
					> footmap_distinction(Now_compass, violaRight))) {
		/*前壁あるけど右壁ないよ→右に曲がるぞ*/
		if (Past_compass == North) { //北向きから右折したよ
			Now_compass = East;
		} else if (Past_compass == East) { //東向きから右折したよ
			Now_compass = South;
		} else if (Past_compass == South) { //南向きから右折したよ
			Now_compass = West;
		} else if (Past_compass == West) { //西向きから右折したよ
			Now_compass = North;
		}

		Right_turn();
		U_turn_Flag = 0;

	} else if ((get_wall(Here_X, Here_Y, God_direction(Now_compass, violaLeft))
			== 0)
			&& (footmap[Here_X][Here_Y]
					> footmap_distinction(Now_compass, violaLeft))) {
		/*左壁がないぞ*/

		if (Past_compass == North) { //北向きから左折したよ
			Now_compass = West;
		} else if (Past_compass == East) { //東向きから左折したよ
			Now_compass = North;
		} else if (Past_compass == South) { //南向きから左折したよ
			Now_compass = East;
		} else if (Past_compass == West) { //西向きから左折したよ
			Now_compass = South;
		}

		Left_turn();
		U_turn_Flag = 0;

	} else {
		//行き止まりやんけ(そっとUターン)
		U_turn();
	}
	Im_here(Now_compass);
	Past_compass = Now_compass;
	Sensor_L_Flag = 0;
	Sensor_Centre_Flag = 0;
	Sensor_R_Flag = 0;
}

void Left_hands_getwall(void) {
	//なんてなめらかー
	wall_distinction(Now_compass);
	MAPCost_Memory();
	Where_I_am();
	if (MLeft.val < MLeft_thre) {
		/*左壁isない*/
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;

		if (Past_compass == North) { //北向きから左折したよ
			Now_compass = West;
		} else if (Past_compass == East) { //東向きから左折したよ
			Now_compass = North;
		} else if (Past_compass == South) { //南向きから左折したよ
			Now_compass = East;
		} else if (Past_compass == West) { //西向きから左折したよ
			Now_compass = South;
		}

		Left_turn();
		U_turn_Flag = 0;

	} else if (Centre.val < Centre_thre) {
		//左壁あるし前壁ないよ→直進だオラ
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 1;
		PORTA.PODR.BIT.B6 = 1;
		U_turn_Flag = 0;
		Linear_motion(180, 5000, 500, 500, 500);

	} else if (MRight.val < MRight_thre) {
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		//右壁だけないよ→右に曲がるぞ

		if (Past_compass == North) { //北向きから右折したよ
			Now_compass = East;
		} else if (Past_compass == East) { //東向きから右折したよ
			Now_compass = South;
		} else if (Past_compass == South) { //南向きから右折したよ
			Now_compass = West;
		} else if (Past_compass == West) { //西向きから右折したよ
			Now_compass = North;
		}

		Right_turn();
		U_turn_Flag = 0;

	} else {
		//行き止まりやんけ(そっとUターン)
		PORTA.PODR.BIT.B3 = 0;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;

		U_turn();
	}
	Im_here(Now_compass);
	Past_compass = Now_compass;
}
