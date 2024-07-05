/*
 * path_generate.c
 *
 *  Created on: 2017/12/22
 *      Author: 菫子
 */
#include "path_generate.h"
#define Path_MAX 256

volatile int Path_X, Path_Y;
volatile int Ending_root, Ending_compressRoot;

viola_move Root[Path_MAX];
int compress_Root[Path_MAX] = { 0 };

int watch_mapcost(void) {
	if (Path_footmap[Path_X][Path_Y] == 0) {
		return 1;
	}
	return 0;
}

void Path_at_here(direction compass) {
	if (compass == North) { //北向き
		Path_Y = Path_Y + 1;
	} else if (compass == East) { //東向き
		Path_X = Path_X + 1;
	} else if (compass == South) { //南向き
		Path_Y = Path_Y - 1;
	} else if (compass == West) { //西向き
		Path_X = Path_X - 1;
	}
}

void Path_Generater(void) {
//なんてなめらかー
	direction before_compass, current_compass;
	int tomato, walk_enum, walk;
	tomato = 0;
	walk = 0;
	walk_enum = 0;
	Path_X = 0;
	Path_Y = 1;
	compress_Root[0] = 1;
	current_compass = North;
	before_compass = North;
	while (1) {

		myprintf("Path_X : %d, Path_Y: %d  %d\n", Path_X, Path_Y,
				current_compass);
		MAPCost_Memory();
		tomato = watch_mapcost();

		if (tomato == 1) {
			Root[walk_enum] = goal_in;
			Ending_root = walk_enum;

			if (compress_Root[walk] > 31) {
				walk++;
			}
			compress_Root[walk] += 1;
			Ending_compressRoot = walk;

			return;
		}

		if ((walk >= Path_MAX) || (walk_enum >= Path_MAX)) {
			myprintf("error! error!\n");
			return;
		}

		if ((get_looking_wall(Path_X, Path_Y,
				God_direction(current_compass, violaFront)) == 0)
				&& (Path_footmap[Path_X][Path_Y]
						> Path_footmap_distinction(current_compass, violaFront))) {
			/*前壁ないよ→直進だオラ*/
			Root[walk_enum] = go_straight;
			walk_enum++;

			if (compress_Root[walk] > 31) {
				walk++;
			}
			compress_Root[walk] += 2;

		} else if ((get_looking_wall(Path_X, Path_Y,
				God_direction(current_compass, violaRight)) == 0)
				&& (Path_footmap[Path_X][Path_Y]
						> Path_footmap_distinction(current_compass, violaRight))) {
			/*前壁あるけど右壁ないよ→右に曲がるぞ*/
			if (before_compass == North) { //北向きから右折したよ
				current_compass = East;
			} else if (before_compass == East) { //東向きから右折したよ
				current_compass = South;
			} else if (before_compass == South) { //南向きから右折したよ
				current_compass = West;
			} else if (before_compass == West) { //西向きから右折したよ
				current_compass = North;
			}
			Root[walk_enum] = turn_Right;
			walk_enum++;

			walk++;
			compress_Root[walk] = 200;

		} else if ((get_looking_wall(Path_X, Path_Y,
				God_direction(current_compass, violaLeft)) == 0)
				&& (Path_footmap[Path_X][Path_Y]
						> Path_footmap_distinction(current_compass, violaLeft))) {
			/*左壁がないぞ*/

			if (before_compass == North) { //北向きから左折したよ
				current_compass = West;
			} else if (before_compass == East) { //東向きから左折したよ
				current_compass = North;
			} else if (before_compass == South) { //南向きから左折したよ
				current_compass = East;
			} else if (before_compass == West) { //西向きから左折したよ
				current_compass = South;
			}
			Root[walk_enum] = turn_Left;
			walk_enum++;

			walk++;
			compress_Root[walk] = 300;

		} else {
			//行き止まりやんけ(そっとUターン)
			if (before_compass == North) { //北向きからUターンしたよ
				current_compass = South;
			} else if (before_compass == East) { //東向きからUターンしたよ
				current_compass = West;
			} else if (before_compass == South) { //南向きからUターンしたよ
				current_compass = North;
			} else if (before_compass == West) { //西向きからUターンしたよ
				current_compass = East;
			}
		}
		Path_at_here(current_compass);
		before_compass = current_compass;
	}
}

void minimum_path_walk(volatile float V_Straight) {
	int apple, pumpkin;
	int potato_pie;
//	Path_Flag = 1;

	target_V = 0;
	for (apple = 0; apple <= Ending_compressRoot; apple++) {
		if (compress_Root[apple] == 0) {
//			myprintf("path end.\n");
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
			wait_ms(500);
			break;
		}

		if (compress_Root[apple] < 31) {
			potato_pie = compress_Root[apple];
			if (compress_Root[apple + 1] == 0){
				Linear_motion(90 * potato_pie, 5000, V_Straight, target_V, 0);
			} else {
				Linear_motion(90 * potato_pie, 5000, V_Straight, target_V, 500);
			}
		} else if (compress_Root[apple] == 200) {
			//右折
			Right_turn();
		} else if (compress_Root[apple] == 300) {
			//左折
			Left_turn();
		}
	}
	Goal_path();

}

void Path_print(void) {
	int apple;

	for (apple = 0; apple < Path_MAX; apple++) {
		if (compress_Root[apple] == 0) {
			myprintf("path end.\n");
			return;
		}
		myprintf("%d: %d  %d\n", apple, compress_Root[apple]);
	}

}

