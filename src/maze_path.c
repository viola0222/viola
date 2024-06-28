/*
 * maze_path.c
 *
 *  Created on: 2017/10/20
 *      Author: 菫子
 */
#include "maze_path.h"

#define Path_all 512

int path[Path_all] = { 0 };
int path_memory[Path_all] = { 0 };
int path_large_turn[Path_all] = { 0 };
direction Path_Past_compass, Path_Now_compass;
int Path_Flag, path_end_memory, path_end_large, Path_end_Flag,
		Path_memory_generate_Flag;
int tail_path_Queue, head_path_Queue;

void Goal_path(void) {

	/*前壁補正*/
//	Linear_motion(30, 5000, 500, target_V, 300);
//	while (Centre.val <= 3340) {
//		Linear_motion(5, 5000, 500, 300, 300);
//	}
//	Linear_motion(5, 5000, 300, 300, 0);
	/*前壁補正終わり*/

//	Linear_motion(90, 5000, 500, 500, 0);
	Ultra_pivot_turn(360, 5000, 300, 0, 0);

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

	PORTA.PODR.BIT.B3 = 0;
	PORTA.PODR.BIT.B4 = 0;
	PORTA.PODR.BIT.B6 = 0;

	if (Comeback_Flag == 0) {

		Ultra_pivot_turn(180, 5000, 500, 0, 0);
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		wait_ms(500);
	} else {
		Ultra_pivot_turn(180, 5000, 500, 0, 0);

		while (PORT3.PIDR.BIT.B5 == 0)
			;
		output_maze_cost();
		Comeback_Flag = 0;
	}
	Goal_Flag = 1;

}

void teuchi_path(void) {

	myprintf("teuchipath\n");
	path[0] = 5;
	path[1] = 200;
	path[2] = 4;
	path[3] = 200;
	path[4] = 200;
	path[5] = 2;
	path[6] = 300;
	path[7] = 300;
	path[8] = 2;
	path[9] = 200;
	path[10] = 200;
	path[11] = 3;

	path_end_memory = 12;
}

void Path_largeturn_compress(void) {
	int apple, grape;

	for (apple = 0; apple < Path_all; apple++) {
		path_large_turn[apple] = 0;

	}
	apple = 0;
	grape = -1;
///XXX 大廻のみのパスにするとバグるよ

	while (apple <= path_end_memory) {

		if (path[apple] < 30) { //0
			//元のpathが直進だったら
			grape++;
			if (path_large_turn[grape] == 0) {
				/*大廻パスに何も代入されていないとき*/
				path_large_turn[grape] = path[apple];
				//とりあえず代入
			} else if (path_large_turn[grape] < (path[apple] - 1)) {
				/*大廻パスに代入されているとき*/
				path_large_turn[grape] = path[apple];
			}
			apple++;			//1
//			grape++;			//1
			myprintf("%d, %d, %d\n", path_large_turn[grape], grape, apple);

			if (path[apple] == 200) {			//1
				//直進の次右折だったら?
				grape++;
				path_large_turn[grape] = path[apple];
				//とりあえず代入
				apple++;				//2
				myprintf("%d, %d, %d\n", path_large_turn[grape], grape, apple);
				if (path[apple] < 30) {				//2
					//さらに直進なら大廻り右折90°->前後の直進を減らす
					grape--;					//0
					if (path_large_turn[grape] < 30) {
						path_large_turn[grape] -= 1;
					}
					if (path_large_turn[grape] != 0) {
						grape++;
					}
					path_large_turn[grape] = 1200;
					grape++;					//2
					path_large_turn[grape] = path[apple] - 1;
//					apple++;
					grape--;
					myprintf("%d, %d, %d\n", path_large_turn[grape], grape,
							apple);
				} else if (path[apple] == 200) {					//3
					//もう一回右折なら

					grape++;
					path_large_turn[grape] = path[apple];
					//とりあえず代入

					apple++;
					myprintf("%d, %d, %d\n", path_large_turn[grape], grape,
							apple);
					if (path[apple] < 30) {					//4
						//さらに直進なら大廻り右折180°->2こ前の直進から-1, 1こ前のpathの値を変える, 最後の直進は-1
						grape -= 2;
						if (path_large_turn[grape] < 30) {
							path_large_turn[grape] -= 1;
						}

						if (path_large_turn[grape] != 0) {
							grape++;
						}
						path_large_turn[grape] = 1400;
						grape++;
						path_large_turn[grape] = path[apple] - 1;

						grape--;
						myprintf("%d, %d, %d\n", path_large_turn[grape], grape,
								apple);
					} else {
						//左折なら(FRRL)->135°ターン?　今は入れない
						grape++;
						path_large_turn[grape] = path[apple];

						apple++;

					}
				} else {
					//左折だったら?(FRL)->斜め　今は入れない
					grape++;
					path_large_turn[grape] = path[apple];

					apple++;
				}

			} else if (path[apple] == 300) {
				//直進の次左折だったら?
				grape++;
				path_large_turn[grape] = path[apple];
				//とりあえず代入
				apple++;

				if (path[apple] < 30) {
					//さらに直進なら大廻り左折90°->前後の直進を減らす
					grape--;
					if (path_large_turn[grape] < 30) {
						path_large_turn[grape] -= 1;
					}
					if (path_large_turn[grape] != 0) {
						grape++;
					}
					path_large_turn[grape] = 2300;
					grape++;
					path_large_turn[grape] = path[apple] - 1;
//					apple++;
					grape--;
				} else if (path[apple] == 300) {
					//もう一回左折なら

					grape++;
					path_large_turn[grape] = path[apple];
					//とりあえず代入
					apple++;

					if (path[apple] < 30) {
						//さらに直進なら大廻り左折180°->2こ前の直進から-1, 1こ前のpathの値を変える, 最後の直進は-1
						grape -= 2;
						if (path_large_turn[grape] < 30) {
							path_large_turn[grape] -= 1;
						}
						if (path_large_turn[grape] != 0) {
							grape++;
						}
						path_large_turn[grape] = 2600;
						grape++;
						path_large_turn[grape] = path[apple] - 1;
//						apple++;
						grape--;
					} else {
						//右折なら(FLLR)->135°ターン?　今は入れない
						grape++;
						path_large_turn[grape] = path[apple];
						apple++;
					}

				} else {
					//左折だったら?(FLR)->斜め　今は入れない
					grape++;
					path_large_turn[grape] = path[apple];
					apple++;
				}
			}

		} else {
			//見てるのが直進以外だったら?->そのまんまいれようね
			grape++;
			path_large_turn[grape] = path[apple];
			apple++;

		}
	}
	path_end_large = grape;
	Path_end_Flag = 1;
}

void Make_path(void) {
	int apple, tomato;
	Path_Flag = 1;

	for (apple = 0; apple < Path_all; apple++) {
		path[apple] = 0;
	}
	tomato = 0;
	apple = 0;
	path_end_memory = 0;
	path[0] = 1;
	while (Path_Flag == 1) {

		wall_distinction(Path_Now_compass);
		MAPCost_Memory();
		tomato = Where_I_am();

		if (tomato == 1) {
			path_end_memory = apple;
			Goal_Flag = 1;
			Path_Flag = 0;
			return;
		}

		if ((get_know_wall(Here_X, Here_Y,
				God_direction(Path_Now_compass, violaFront)) == 1)
				&& (get_wall(Here_X, Here_Y,
						God_direction(Path_Now_compass, violaFront)) == 0)
				&& (footmap[Here_X][Here_Y]
						> footmap_distinction(Path_Now_compass, violaFront))) {
			/*前壁ないよ→直進だオラ*/

			path[apple] = 2;

		} else if ((get_know_wall(Here_X, Here_Y,
				God_direction(Path_Now_compass, violaRight)) == 1)
				&& (get_wall(Here_X, Here_Y,
						God_direction(Path_Now_compass, violaRight)) == 0)
				&& (footmap[Here_X][Here_Y]
						> footmap_distinction(Path_Now_compass, violaRight))) {
			/*前壁あるけど右壁ないよ→右に曲がるぞ*/

			if (Path_Past_compass == North) { //北向きから右折したよ
				Path_Now_compass = East;
			} else if (Path_Past_compass == East) { //東向きから右折したよ
				Path_Now_compass = South;
			} else if (Path_Past_compass == South) { //南向きから右折したよ
				Path_Now_compass = West;
			} else if (Path_Past_compass == West) { //西向きから右折したよ
				Path_Now_compass = North;
			}

			path[apple] = 20;

		} else if ((get_know_wall(Here_X, Here_Y,
				God_direction(Path_Now_compass, violaLeft)) == 1)
				&& (get_wall(Here_X, Here_Y,
						God_direction(Path_Now_compass, violaLeft)) == 0)
				&& (footmap[Here_X][Here_Y]
						> footmap_distinction(Path_Now_compass, violaLeft))) {
			/*左壁がないぞ*/

			if (Path_Past_compass == North) { //北向きから左折したよ
				Path_Now_compass = West;
			} else if (Path_Past_compass == East) { //東向きから左折したよ
				Path_Now_compass = North;
			} else if (Path_Past_compass == South) { //南向きから左折したよ
				Path_Now_compass = East;
			} else if (Path_Past_compass == West) { //西向きから左折したよ
				Path_Now_compass = South;
			}

			path[apple] = 30;

		} else {
			//行き止まりやんけ(そっとUターン)

			if (Path_Past_compass == North) { //北向きからUターンしたよ
				Path_Now_compass = South;
			} else if (Path_Past_compass == East) { //東向きからUターンしたよ
				Path_Now_compass = West;
			} else if (Path_Past_compass == South) { //南向きからUターンしたよ
				Path_Now_compass = North;
			} else if (Path_Past_compass == West) { //西向きからUターンしたよ
				Path_Now_compass = East;
			}

		}
		Im_here(Path_Now_compass);
		Path_Past_compass = Path_Now_compass;
		if (apple < Path_all) {
			apple++;
		} else {
			return;
		}
	}
}

void Path_output(void) {
	int apple;
	Path_Flag = 1;

	for (apple = 0; apple < Path_all; apple++) {
		if (path[apple] == 0) {
			myprintf("path end.\n");
			Path_Flag = 0;
			return;
		}
		myprintf("%d: %d  %d\n", apple, path[apple], path_memory[apple]);
	}

}

void Path_large_output(void) {
	int apple;
	Path_Flag = 1;

	for (apple = 0; apple < Path_all; apple++) { //Path_all
		if (path_large_turn[apple] == 0) {
			myprintf("path end.\n");
			Path_Flag = 0;
			return;
		}
		myprintf("%d: %d  %d\n", apple, path_large_turn[apple],
				path_memory[apple]);
	}
	myprintf("path end.\n");

}

void minimum_walk(void) {
	int apple;
//	Path_Flag = 1;

//	Linear_motion(90, 5000, 500, 0, 500);
	target_V = 0;
	for (apple = 0; apple <= path_end_memory; apple++) {
		if (path[apple] == 0) {
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

		if (path[apple] == 2) {
			Linear_motion(180, 5000, 500, target_V, 500);

		} else if (path[apple] == 20) {
			//右折
			turn_R_Flag = 1;
			Linear_motion(2, 5000, 500, 500, 500);
			Slalom(90, -10000, -400, 500);
			Linear_motion(5, 5000, 500, 500, 500);
			turn_R_Flag = 0;

		} else if (path[apple] == 30) {
			//左折
			turn_L_Flag = 1;
			Linear_motion(24, 5000, 500, 500, 500);
			Slalom(90, 10000, 450, 500);
			Linear_motion(13, 5000, 500, 500, 500);
			turn_L_Flag = 0;

		}
	}
	Goal();

}

void Make_half_path(void) {
	int apple, tomato, pasta;
	Path_Flag = 1;
	Comeback_Flag = 0;
	Here_X = 0;
	Here_Y = 1;
	Path_Past_compass = North;
	Path_Now_compass = North;

	for (apple = 0; apple < Path_all; apple++) {
		path[apple] = 0;
	}

	tomato = 0;
	apple = 0;
	pasta = 0;
	path_end_memory = 1;
	path[0] = 1;

	while ((Path_Flag == 1) || (footmap[Here_X][Here_Y] != 255)) {
		myprintf("%d %d\n", Here_X, Here_Y);
//		wall_distinction(Path_Now_compass);
		MAPCost_Memory();
		//	Path_MAP(); //これあとでコメントアウト
		tomato = Where_I_am();

		if (tomato == 1) {
			if (path[apple] > 30) {
				apple++;
			}
			path_end_memory = apple;
			path[apple] += 1; //ゴール区画の中心座標
			Goal_Flag = 1;
			if (Path_memory_generate_Flag == 0) {
				//path一回しか作らせないマン
				for (pasta = 0; pasta <= path_end_memory; pasta++) {
					path_memory[pasta] = path[pasta];
				}
			}
			Path_memory_generate_Flag = 1;
			Path_Flag = 0;

			return;
		}

		if ((get_know_wall(Here_X, Here_Y,
				God_direction(Path_Now_compass, violaFront)) == 1)
				&& (get_wall(Here_X, Here_Y,
						God_direction(Path_Now_compass, violaFront)) == 0)
				&& (footmap[Here_X][Here_Y]
						> footmap_distinction(Path_Now_compass, violaFront))) {
			/*前壁ないよ→直進だオラ*/

			if (path[apple] > 30) {
				apple++;
			}
			path[apple] += 2;
			//半区画*2進む

		} else if ((get_know_wall(Here_X, Here_Y,
				God_direction(Path_Now_compass, violaRight)) == 1)
				&& (get_wall(Here_X, Here_Y,
						God_direction(Path_Now_compass, violaRight)) == 0)
				&& (footmap[Here_X][Here_Y]
						> footmap_distinction(Path_Now_compass, violaRight))) {
			/*前壁あるけど右壁ないよ→右に曲がるぞ*/

			if (Path_Past_compass == North) { //北向きから右折したよ
				Path_Now_compass = East;
			} else if (Path_Past_compass == East) { //東向きから右折したよ
				Path_Now_compass = South;
			} else if (Path_Past_compass == South) { //南向きから右折したよ
				Path_Now_compass = West;
			} else if (Path_Past_compass == West) { //西向きから右折したよ
				Path_Now_compass = North;
			}

			apple++;
			path[apple] = 200;

		} else if ((get_know_wall(Here_X, Here_Y,
				God_direction(Path_Now_compass, violaLeft)) == 1)
				&& (get_wall(Here_X, Here_Y,
						God_direction(Path_Now_compass, violaLeft)) == 0)
				&& (footmap[Here_X][Here_Y]
						> footmap_distinction(Path_Now_compass, violaLeft))) {
			/*左壁がないぞ*/

			if (Path_Past_compass == North) { //北向きから左折したよ
				Path_Now_compass = West;
			} else if (Path_Past_compass == East) { //東向きから左折したよ
				Path_Now_compass = North;
			} else if (Path_Past_compass == South) { //南向きから左折したよ
				Path_Now_compass = East;
			} else if (Path_Past_compass == West) { //西向きから左折したよ
				Path_Now_compass = South;
			}

			apple++;
			path[apple] = 300;

		} else {
			//行き止まりやんけ(そっとUターン)

			if (Path_Past_compass == North) { //北向きからUターンしたよ
				Path_Now_compass = South;
			} else if (Path_Past_compass == East) { //東向きからUターンしたよ
				Path_Now_compass = West;
			} else if (Path_Past_compass == South) { //南向きからUターンしたよ
				Path_Now_compass = North;
			} else if (Path_Past_compass == West) { //西向きからUターンしたよ
				Path_Now_compass = East;
			}

		}
		Im_here(Path_Now_compass);
		Path_Past_compass = Path_Now_compass;
		if (apple >= Path_all) {
			return;
		}
	}
	return;
}

void minimum_half_walk(volatile float V_Straight) {
	int apple, pumpkin;
	int potato_pie;
//	Path_Flag = 1;

	target_V = 0;
	for (apple = 0; apple <= path_end_memory; apple++) {
		if (path_memory[apple] == 0) {
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

		if (path_memory[apple] < 31) {
			potato_pie = path_memory[apple];
			Linear_motion(90 * potato_pie, 5000, V_Straight, target_V, 500);

		} else if (path_memory[apple] == 200) {
			//右折
			Right_turn();
		} else if (path_memory[apple] == 300) {
			//左折
			Left_turn();
		}
	}
	Goal_path();

}

void Large_turn_walk(volatile float V_Straight, volatile float V_Curve) {
	int apple, pumpkin;
	int potato_pie;
//	Path_Flag = 1;

	target_V = 0;
	for (apple = 0; apple <= path_end_large; apple++) {
		if (path_large_turn[apple] == 0) {
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

		if (path_large_turn[apple] < 31) {
			potato_pie = path_large_turn[apple];
			if (path_large_turn[apple + 1] > 1000) {
				Linear_motion(90 * potato_pie, 10000, V_Straight, target_V,
						V_Curve);
			} else {
				Linear_motion(90 * potato_pie, 10000, V_Straight, target_V,
						500);
			}

		} else if (path_large_turn[apple] == 1200) {
			/*右大廻90°*/
			Right_large_turn(V_Curve);
		} else if (path_large_turn[apple] == 1400) {
			/*右大廻180°*/
			Right_large_U_turn(V_Curve);
		} else if (path_large_turn[apple] == 2300) {
			/*左大廻90°*/
			Left_large_turn(V_Curve);
		} else if (path_large_turn[apple] == 2600) {
			/*左大廻180°*/
			Left_large_U_turn(V_Curve);
		} else if (path_large_turn[apple]) {
			//右折
			Right_turn();
		} else if (path_large_turn[apple]) {
			//左折
			Left_turn();
		}
	}
	Goal_path();

}

