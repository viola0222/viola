/*
 * maze_map.c
 *
 *  Created on: 2017/09/27
 *      Author: 菫子
 */
#include "maze_map.h"

unsigned short footmap[16][16] = { 255 };
unsigned short Path_footmap[16][16] = { 255 };
unsigned short sideX_wall[17][16] = { 32767 }; //X方向の壁
unsigned short sideY_wall[16][17] = { 32767 }; //Y方向の壁

unsigned short queue[40][3]; //リスト
unsigned short path_queue[40][3]; //リスト

unsigned short column_wall[15]; //縦壁
unsigned short row_wall[15]; //横壁
unsigned short known_column_wall[15]; //未知の縦壁
unsigned short known_row_wall[15]; //未知の横壁
unsigned short memory_column_wall[15]; //記憶しとくよ縦壁
unsigned short memory_row_wall[15]; //記憶しとくよ横壁

typedef struct {
	signed int val; //現在のセンサーの値
	signed int dif; //微分値(傾き)
	signed int threshold; //壁のあるなしの閾値
	signed int before; //前回のセンサー値
	signed int ref; //reference
} Sensor;

extern Sensor Centre, MRight, MLeft;
extern volatile int U_turn_Flag;
extern int Circuit_Flag, Circuit_count, Path_Flag;
;
extern direction Past_compass, Now_compass;
extern volatile int Path_X, Path_Y;

volatile int Here_X, Here_Y, Comeback_Flag, Goal_Flag;
int head_Queue, tail_Queue, path_head_Queue, path_tail_Queue;

void Im_here(direction);

int get_looking_wall(int x, int y, direction wall_compass) {

	if ((get_wall(x, y, wall_compass) == 0)
			&& (get_know_wall(x, y, wall_compass) == 1)) {
		/*既知壁かつ壁がないのなら*/
		return 0;
	} else {
		return 1;
	}

}

void Goal(void) {

	/*前壁補正*/
//	Linear_motion(30, 5000, 500, target_V, 300);
//	while (Centre.val <= 3340) {
//		Linear_motion(5, 5000, 500, 300, 300);
//	}
//	Linear_motion(5, 5000, 300, 300, 0);
	/*前壁補正終わり*/
	Linear_motion(90, 5000, 500, 500, 0);
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

	if (U_turn_Flag == 1) {
		while (PORT3.PIDR.BIT.B5 == 0)
			;
		output_maze_cost();
		while (1)
			;
	} else if (Comeback_Flag == 0) {
		if (Past_compass == North) { //北向きからUターンしたよ
			Now_compass = South;
		} else if (Past_compass == East) { //東向きからUターンしたよ
			Now_compass = West;
		} else if (Past_compass == South) { //南向きからUターンしたよ
			Now_compass = North;
		} else if (Past_compass == West) { //西向きからUターンしたよ
			Now_compass = East;
		}
		Ultra_pivot_turn(180, 5000, 300, 0, 0);
		PORTA.PODR.BIT.B3 = 1;
		PORTA.PODR.BIT.B4 = 0;
		PORTA.PODR.BIT.B6 = 0;
		Past_compass = Now_compass;
		wait_ms(500);
	} else {
		Ultra_pivot_turn(180, 5000, 300, 0, 0);
		if (Past_compass == North) { //北向きからUターンしたよ
			Now_compass = South;
		} else if (Past_compass == East) { //東向きからUターンしたよ
			Now_compass = West;
		} else if (Past_compass == South) { //南向きからUターンしたよ
			Now_compass = North;
		} else if (Past_compass == West) { //西向きからUターンしたよ
			Now_compass = East;
		}

		while (PORT3.PIDR.BIT.B5 == 0)
			;
		output_maze_cost();
		Comeback_Flag = 0;
	}
	Goal_Flag = 1;

}

int Where_I_am(void) {
	if (Path_Flag == 1) {
		if (footmap[Here_X][Here_Y] == 0) { //こいつがPath_footmapになってたせいだわ
			return 1;
		}
		return 0;
	}
	if (footmap[Here_X][Here_Y] == 0) {
		Goal();
		return 1;
	}
	if ((Circuit_Flag == 1) && ((Here_X == 0) && (Here_Y == 1))) {
		Circuit_count++;
	}
	if ((Circuit_Flag == 1) && (Circuit_count >= 3)) {
		Goal();
		return 1;
	}
	return 0;
}

void Im_here(direction compass) {
	if (compass == North) { //北向き
		Here_Y = Here_Y + 1;
	} else if (compass == East) { //東向き
		Here_X = Here_X + 1;
	} else if (compass == South) { //南向き
		Here_Y = Here_Y - 1;
	} else if (compass == West) { //西向き
		Here_X = Here_X - 1;
	}
}

void FIFO_Queue_push(int cost, int x, int y) {
	queue[tail_Queue][0] = cost;
	queue[tail_Queue][1] = x;
	queue[tail_Queue][2] = y;
	tail_Queue++;

	if (tail_Queue == 40) {
		tail_Queue = 0;
	}
}

void FIFO_Queue_pop(void) {

	head_Queue++;
	if (head_Queue == 40) {
		head_Queue = 0;
	}
}

void path_FIFO_Queue_push(int cost, int x, int y) {
	path_queue[path_tail_Queue][0] = cost;
	path_queue[path_tail_Queue][1] = x;
	path_queue[path_tail_Queue][2] = y;
	tail_Queue++;

	if (path_tail_Queue == 40) {
		path_tail_Queue = 0;
	}
}

void path_FIFO_Queue_pop(void) {

	path_head_Queue++;
	if (path_head_Queue == 40) {
		path_head_Queue = 0;
	}
}

void MAPCost_Memory(void) {
	direction wall_compass;
	int x, y;
	unsigned short cost;

	cost = 0;

	for (x = 0; x < 16; x++) {
		for (y = 0; y < 16; y++) {
			footmap[x][y] = 255;
		}
	}
	if (Comeback_Flag == 0) {
		x = Goal_X;
		y = Goal_Y;
	} else {
		x = 0;
		y = 0;
	}

	footmap[x][y] = 0;

	FIFO_Queue_push(0, x, y);

	while (head_Queue != tail_Queue) {
		cost = queue[head_Queue][0];
		x = queue[head_Queue][1];
		y = queue[head_Queue][2];
		FIFO_Queue_pop();

		wall_compass = North;
		if (get_wall(x, y, wall_compass) == 0) {
			/*今見てる座標の北に壁がなかったら*/
			if (footmap[x][y + 1] > (cost + 1)) { //現在入っている値よりも代入する値が小さかったら
				footmap[x][y + 1] = cost + 1;
				FIFO_Queue_push(cost + 1, x, y + 1);
			}
		}
		wall_compass = East;
		if (get_wall(x, y, wall_compass) == 0) {
			/*今見てる座標の東に壁がなかったら*/
			if (footmap[x + 1][y] > (cost + 1)) { //現在入っている値よりも代入する値が小さかったら比較
				footmap[x + 1][y] = cost + 1;
				FIFO_Queue_push(cost + 1, x + 1, y);
			}
		}
		wall_compass = South;
		if (get_wall(x, y, wall_compass) == 0) {
			/*今見てる座標の南に壁がなかったら*/
			if (footmap[x][y - 1] > (cost + 1)) { //現在入っている値よりも代入する値が小さかったら
				footmap[x][y - 1] = cost + 1;
				FIFO_Queue_push(cost + 1, x, y - 1);
			}
		}
		wall_compass = West;
		if (get_wall(x, y, wall_compass) == 0) {
			/*今見てる座標の西に壁がなかったら*/
			if (footmap[x - 1][y] > (cost + 1)) { //現在入っている値よりも代入する値が小さかったら
				footmap[x - 1][y] = cost + 1;
				FIFO_Queue_push(cost + 1, x - 1, y);
			}
		}
	}
	wall_compass = North;
}

short get_footmap(int x, int y, direction map_compass) {
	short pasta;
	/*迷路のコスト情報をほかのところから参照するときに取得する関数*/
	/*x, y 座標
	 * map_compass 見たい座標の方角*/
	if (((x == 0) && (map_compass == West))
			|| ((x == 15) && (map_compass == East))
			|| ((y == 0) && (map_compass == South))
			|| ((y == 15) && (map_compass == North))) {
		//範囲外アクセス
		return 255;
	}

	if ((map_compass == East) || (map_compass == West)) {
		//横の座標のコスト見たいです
		if (map_compass == West) {
			return get_footmap(x - 2, y, East);
		} else {
			pasta = footmap[x + 1][y];
			return pasta;
		}
	} else {
		//上下の座標のコスト見たいです
		if (map_compass == South) {
			return get_footmap(x, y - 2, North);
		} else {
			pasta = footmap[x][y + 1];
			return pasta;
		}
	}
//	return 0;
}

short get_Path_footmap(int x, int y, direction map_compass) {
	short pasta;
	/*迷路のコスト情報をほかのところから参照するときに取得する関数*/
	/*x, y 座標
	 * map_compass 見たい座標の方角*/
	if (((x == 0) && (map_compass == West))
			|| ((x == 15) && (map_compass == East))
			|| ((y == 0) && (map_compass == South))
			|| ((y == 15) && (map_compass == North))) {
		//範囲外アクセス
		return 255;
	}

	if ((map_compass == East) || (map_compass == West)) {
		//横の座標のコスト見たいです
		if (map_compass == West) {
			return get_Path_footmap(x - 2, y, East);
		} else {
			pasta = Path_footmap[x + 1][y];
			return pasta;
		}
	} else {
		//上下の座標のコスト見たいです
		if (map_compass == South) {
			return get_Path_footmap(x, y - 2, North);
		} else {
			pasta = Path_footmap[x][y + 1];
			return pasta;
		}
	}
//	return 0;
}

void Path_MAP(void) {
	direction wall_compass;
	int x, y;
	unsigned short cost;

	cost = 0;

	for (x = 0; x < 16; x++) {
		for (y = 0; y < 16; y++) {
			Path_footmap[x][y] = 255;
		}
	}
	if (Comeback_Flag == 0) {
		x = Goal_X;
		y = Goal_Y;
	} else {
		x = 0;
		y = 0;
	}

	Path_footmap[x][y] = 0;

	FIFO_Queue_push(0, x, y);

	while (head_Queue != tail_Queue) {
		cost = queue[head_Queue][0];
		x = queue[head_Queue][1];
		y = queue[head_Queue][2];
		FIFO_Queue_pop();

		wall_compass = North;
//		if (get_looking_wall(x, y, wall_compass) == 0) {
//			/*今見てる座標の北に壁がなかったら*/
//			if (Path_footmap[x][y + 1] > (cost + 1)) { //現在入っている値よりも代入する値が小さかったら
//				Path_footmap[x][y + 1] = cost + 1;
//				path_FIFO_Queue_push(cost + 1, x, y + 1);
//			}
//		}
//		wall_compass = East;
//		if (get_looking_wall(x, y, wall_compass) == 0) {
//			/*今見てる座標の東に壁がなかったら*/
//			if (Path_footmap[x + 1][y] > (cost + 1)) { //現在入っている値よりも代入する値が小さかったら比較
//				Path_footmap[x + 1][y] = cost + 1;
//				path_FIFO_Queue_push(cost + 1, x + 1, y);
//			}
//		}
//		wall_compass = South;
//		if (get_looking_wall(x, y, wall_compass) == 0) {
//			/*今見てる座標の南に壁がなかったら*/
//			if (Path_footmap[x][y - 1] > (cost + 1)) { //現在入っている値よりも代入する値が小さかったら
//				Path_footmap[x][y - 1] = cost + 1;
//				path_FIFO_Queue_push(cost + 1, x, y - 1);
//			}
//		}
//		wall_compass = West;
//		if (get_looking_wall(x, y, wall_compass) == 0) {
//			/*今見てる座標の西に壁がなかったら*/
//			if (Path_footmap[x - 1][y] > (cost + 1)) { //現在入っている値よりも代入する値が小さかったら
//				Path_footmap[x - 1][y] = cost + 1;
//				path_FIFO_Queue_push(cost + 1, x - 1, y);
//			}
//		}
//	}
//
//
		if ((get_know_wall(x, y, wall_compass) == 1)
				&& (get_wall(x, y, wall_compass) == 0)) {
			/*今見てる座標の北に壁がなかったら*/
			if (Path_footmap[x][y + 1] > (cost + 1)) { //現在入っている値よりも代入する値が小さかったら
				Path_footmap[x][y + 1] = cost + 1;
				FIFO_Queue_push(cost + 1, x, y + 1);
			}
		}
		wall_compass = East;
		if (((get_know_wall(x, y, wall_compass) == 1)
				&& get_wall(x, y, wall_compass) == 0)) {
			/*今見てる座標の東に壁がなかったら*/
			if (Path_footmap[x + 1][y] > (cost + 1)) { //現在入っている値よりも代入する値が小さかったら比較
				Path_footmap[x + 1][y] = cost + 1;
				FIFO_Queue_push(cost + 1, x + 1, y);
			}
		}
		wall_compass = South;
		if (((get_know_wall(x, y, wall_compass) == 1)
				&& get_wall(x, y, wall_compass) == 0)) {
			/*今見てる座標の南に壁がなかったら*/
			if (Path_footmap[x][y - 1] > (cost + 1)) { //現在入っている値よりも代入する値が小さかったら
				Path_footmap[x][y - 1] = cost + 1;
				FIFO_Queue_push(cost + 1, x, y - 1);
			}
		}
		wall_compass = West;
		if (((get_know_wall(x, y, wall_compass) == 1)
				&& get_wall(x, y, wall_compass) == 0)) {
			/*今見てる座標の西に壁がなかったら*/
			if (Path_footmap[x - 1][y] > (cost + 1)) { //現在入っている値よりも代入する値が小さかったら
				Path_footmap[x - 1][y] = cost + 1;
				FIFO_Queue_push(cost + 1, x - 1, y);
			}
		}
	}
	wall_compass = North;
}

short footmap_distinction(direction compass, viola_looking sum) {
	/*歩数判別関数
	 * compass:現在の方向
	 * sum:見てるほう(左前右)*/
	direction look_cost;

	look_cost = God_direction(compass, sum);
	return get_footmap(Here_X, Here_Y, look_cost);
}

short Path_footmap_distinction(direction compass, viola_looking sum) {
	/*歩数判別関数
	 * compass:現在の方向
	 * sum:見てるほう(左前右)*/
	direction look_cost;

	look_cost = God_direction(compass, sum);
	return get_Path_footmap(Path_X, Path_Y, look_cost);
}

void set_wall(int x, int y, direction wall_compass) {
	/*壁情報を入れる関数*/
	/*x, y 座標
	 * wall_compass 入れたい壁の方角*/
	if (((x == 0) && (wall_compass == West))
			|| ((x == 15) && (wall_compass == East))
			|| ((y == 0) && (wall_compass == South))
			|| ((y == 15) && (wall_compass == North))) {
		//範囲外アクセス
		return;
	}

	if ((wall_compass == East) || (wall_compass == West)) {
		//縦壁見てます
		if (wall_compass == West) {
			set_wall(x - 1, y, East);
//			column_wall[x - 1] |= (1 << (y));
		} else {
			column_wall[x] |= (1 << (y));
		}
	} else {
		//横壁見てます
		if (wall_compass == South) {
			set_wall(x, y - 1, North);
//			row_wall[y - 1] |= (1 << (x));
		} else {
			row_wall[y] |= (1 << (x));
		}
	}
}

int get_wall(int x, int y, direction wall_compass) {
	short pasta;
	/*壁情報をほかのところから参照するときに取得する関数*/
	/*x, y 座標
	 * wall_compass 見たい壁の方角*/
	if (((x == 0) && (wall_compass == West))
			|| ((x == 15) && (wall_compass == East))
			|| ((y == 0) && (wall_compass == South))
			|| ((y == 15) && (wall_compass == North))) {
		//範囲外アクセス
		return 1;
	}
	if ((x == 0) && (y == 0) && (wall_compass == East)) {
		//0, 0の右壁
		return 1;
	}
	if ((x == 0) && (y == 0) && (wall_compass == North)) {
		//0, 0の前壁
		return 0;
	}

	if ((wall_compass == East) || (wall_compass == West)) {
		//縦壁見たいです
		if (wall_compass == West) {
			return get_wall(x - 1, y, East);
		} else {
			pasta = (column_wall[x] & (1 << (y)));
			if (pasta != 0x0) {
				return 1;
			} else {
				return 0;
			}

		}
	} else {
		//横壁見たいです
		if (wall_compass == South) {
			return get_wall(x, y - 1, North);
		} else {
			pasta = (row_wall[y] & (1 << (x)));
			if (pasta != 0x0) {
				return 1;
			} else {
				return 0;
			}

		}
	}
	return 0;
}

void set_know_wall(int x, int y, direction wall_compass) {
	/*既知壁情報を入れる関数*/
	/*x, y 座標
	 * wall_compass 入れたい壁の向き
	 * 見たら1にするよ*/
	if (((x == 0) && (wall_compass == West))
			|| ((x == 15) && (wall_compass == East))
			|| ((y == 0) && (wall_compass == South))
			|| ((y == 15) && (wall_compass == North))) {
		//範囲外アクセス
		return;
	}

	if ((wall_compass == East) || (wall_compass == West)) {
		//縦壁見てます
		if (wall_compass == West) {
			set_know_wall(x - 1, y, East);
		} else {
			known_column_wall[x] |= (1 << (y));
		}
	} else {
		//横壁見てます
		if (wall_compass == South) {
			set_know_wall(x, y - 1, North);
		} else {
			known_row_wall[y] |= (1 << (x));
		}
	}

}

int get_know_wall(int x, int y, direction wall_compass) {
	short pasta;
	/*既知壁情報をほかのところから参照するときに取得する関数*/
	/*x, y 座標
	 * wall_compass 見たい壁の方角*/
	/*0:未知壁　1:既知壁*/
	if (((x == 0) && (wall_compass == West))
			|| ((x == 15) && (wall_compass == East))
			|| ((y == 0) && (wall_compass == South))
			|| ((y == 15) && (wall_compass == North))) {
		//範囲外アクセス
		return 1;
	}
	if ((x == 0) && (y == 0) && (wall_compass == East)) {
		//0, 0の右壁
		return 1;
	}

	if ((x == 0) && (y == 0) && (wall_compass == North)) {
		//0, 0の前壁
		return 1;
	}

	if ((wall_compass == East) || (wall_compass == West)) {
		//縦壁見たいです
		if (wall_compass == West) {
			return get_know_wall(x - 1, y, East);
		} else {
			pasta = (known_column_wall[x] & (1 << (y)));
			if (pasta != 0x0) {
				return 1;
			} else {
				return 0;
			}

		}
	} else {
		//横壁見たいです
		if (wall_compass == South) {
			return get_know_wall(x, y - 1, North);
		} else {
			pasta = (known_row_wall[y] & (1 << (x)));
			if (pasta != 0x0) {
				return 1;
			} else {
				return 0;
			}

		}
	}
	return 0;
}

void memory_wall(void) {
	int kabe = 0;

	if (footmap[Here_X][Here_Y] != 255) {
		for (kabe = 0; kabe < 15; kabe++) {
			if (memory_column_wall[kabe] != column_wall[kabe]) {
				memory_column_wall[kabe] = column_wall[kabe];
			}
			if (memory_row_wall[kabe] != row_wall[kabe]) {
				memory_row_wall[kabe] = row_wall[kabe];
			}
		}
	}
}

direction God_direction(direction compass, viola_looking sum) {
	direction allow;
	if (compass == North) {
		if (sum == violaLeft) { //左壁見てるよ
			allow = West;
		} else if (sum == violaFront) { //前壁見てるよ
			allow = North;
		} else if (sum == violaRight) { //右壁見てるよ
			allow = East;
		}
	} else if (compass == East) {
		if (sum == violaLeft) { //左壁見てるよ
			allow = North;
		} else if (sum == violaFront) { //前壁見てるよ
			allow = East;
		} else if (sum == violaRight) { //右壁見てるよ
			allow = South;
		}
	} else if (compass == South) {
		if (sum == violaLeft) { //左壁見てるよ
			allow = East;
		} else if (sum == violaFront) { //前壁見てるよ
			allow = South;
		} else if (sum == violaRight) { //右壁見てるよ
			allow = West;
		}
	} else {
		if (sum == violaLeft) { //左壁見てるよ
			allow = South;
		} else if (sum == violaFront) { //前壁見てるよ
			allow = West;
		} else if (sum == violaRight) { //右壁見てるよ
			allow = North;
		}
	}

	return allow;
}

void wall_distinction(direction compass) {
	/*壁判別関数*/
	direction lookwall;

	if (MLeft.val > MLeft_thre) { //左壁あるよ
		lookwall = God_direction(compass, violaLeft);
//		if (get_know_wall(Here_X, Here_Y, lookwall) == 0) {
		set_wall(Here_X, Here_Y, lookwall);
//		}
		Sensor_L_Flag = 1;
	}
	if (Centre.val > Centre_thre) { //前壁あるよ
		lookwall = God_direction(compass, violaFront);
//		if (get_know_wall(Here_X, Here_Y, lookwall) == 0) {
		set_wall(Here_X, Here_Y, lookwall);
//		}
		Sensor_Centre_Flag = 1;
	}
	if (MRight.val > MRight_thre) { //右壁あるよ
		lookwall = God_direction(compass, violaRight);
//		if (get_know_wall(Here_X, Here_Y, lookwall) == 0) {
		set_wall(Here_X, Here_Y, lookwall);
//		}
		Sensor_R_Flag = 1;
	}
}

void output_maze(void) {
	int column, length;

	MAPCost_Memory();
	for (column = 0; column < 16; column++) {
		myprintf("+");
		myprintf("- - -");
	}
	myprintf("+\n");

	for (length = 15; length >= 0; length--) {
		myprintf("|     ");
		for (column = 0; column < 16; column++) {
			//縦壁
			if (get_wall(column, length, East) == 1) {
				myprintf("|");
			} else {
				myprintf(" ");
			}
			myprintf("     ");
		}
		myprintf("\n");

		myprintf("+");
		for (column = 0; column < 16; column++) {
			//横壁表示
			if (get_wall(column, length, South) == 1) {
				myprintf("- - -");
			} else {
				myprintf("     ");
			}
			myprintf("+");
		}
		myprintf("\n");

	}
}

void output_maze_cost(void) {
	int column, length;

	MAPCost_Memory();
	for (column = 0; column < 16; column++) {
		myprintf("+");
		myprintf("- - -");
	}
	myprintf("+\n");

	for (length = 15; length >= 0; length--) {
		myprintf("|");
		for (column = 0; column < 16; column++) {
			//縦壁
			myprintf("%5d", footmap[column][length]);
			if (get_wall(column, length, East) == 1) {
				myprintf("|");
			} else {
				myprintf(" ");
			}

		}
		myprintf("\n");

		myprintf("+");
		for (column = 0; column < 16; column++) {
			//横壁表示
			if (get_wall(column, length, South) == 1) {
				myprintf("- - -");
			} else {
				myprintf("     ");
			}
			myprintf("+");
		}
		myprintf("\n");

	}
}

void output_Path_maze_cost(void) {
	int column, length;

	Path_MAP();
	for (column = 0; column < 16; column++) {
		myprintf("+");
		myprintf("- - -");
	}
	myprintf("+\n");

	for (length = 15; length >= 0; length--) {
		myprintf("|");
		for (column = 0; column < 16; column++) {
			//縦壁
			myprintf("%5d", Path_footmap[column][length]);
			if (((get_know_wall(column, length, East) == 1)
					&& (get_wall(column, length, East) == 1))
					|| (get_know_wall(column, length, East) == 0)) {
				myprintf("|");
			} else {
				myprintf(" ");
			}

		}
		myprintf("\n");

		myprintf("+");
		for (column = 0; column < 16; column++) {
			//横壁表示
			if (((get_know_wall(column, length, South) == 1)
					&& (get_wall(column, length, South) == 1))
					|| (get_know_wall(column, length, South) == 0)) {
				myprintf("- - -");
			} else {
				myprintf("     ");
			}
			myprintf("+");
		}
		myprintf("\n");

	}
}

//自己流(入れてない)
void MAP_WALL_REKO(direction compass) {
	if (compass == North) { //北向き
		if (MLeft.val > MLeft_thre) { //左壁あるよ
			sideY_wall[Here_Y][Here_X] += 200;
		} else {
			sideY_wall[Here_Y][Here_X] -= 200;
		}
		if (Centre.val > Centre_thre) { //前壁あるよ
			sideX_wall[Here_Y + 1][Here_X] += 200;
		} else {
			sideX_wall[Here_Y + 1][Here_X] -= 200;
		}
		if (MRight.val > MRight_thre) { //右壁あるよ
			sideY_wall[Here_Y][Here_X + 1] += 200;
		} else {
			sideY_wall[Here_Y][Here_X + 1] -= 200;
		}

	} else if (compass == East) { //東向き
		if (MLeft.val > MLeft_thre) { //左壁あるよ
			sideX_wall[Here_Y + 1][Here_X] += 200;
		} else {
			sideX_wall[Here_Y + 1][Here_X] -= 200;
		}
		if (Centre.val > Centre_thre) { //前壁あるよ
			sideY_wall[Here_Y][Here_X + 1] += 200;
		} else {
			sideY_wall[Here_Y][Here_X + 1] -= 200;
		}
		if (MRight.val > MRight_thre) { //右壁あるよ
			sideX_wall[Here_Y][Here_X] += 200;
		} else {
			sideX_wall[Here_Y][Here_X] -= 200;
		}

	} else if (compass == South) { //南向き
		if (MLeft.val > MLeft_thre) { //左壁あるよ
			sideY_wall[Here_Y][Here_X + 1] += 200;
		} else {
			sideY_wall[Here_Y][Here_X + 1] -= 200;
		}
		if (Centre.val > Centre_thre) { //前壁あるよ
			sideX_wall[Here_Y][Here_X] += 200;
		} else {
			sideX_wall[Here_Y][Here_X] -= 200;
		}
		if (MRight.val > MRight_thre) { //右壁あるよ
			sideY_wall[Here_Y][Here_X] += 200;
		} else {
			sideY_wall[Here_Y][Here_X] -= 200;
		}

	} else if (compass == West) { //西向き
		if (MLeft.val > MLeft_thre) { //左壁あるよ
			sideX_wall[Here_Y][Here_X] += 200;
		} else {
			sideX_wall[Here_Y][Here_X] -= 200;
		}
		if (Centre.val > Centre_thre) { //前壁あるよ
			sideY_wall[Here_Y][Here_X] += 200;
		} else {
			sideY_wall[Here_Y][Here_X] -= 200;
		}
		if (MRight.val > MRight_thre) { //右壁あるよ
			sideX_wall[Here_Y + 1][Here_X] += 200;
		} else {
			sideX_wall[Here_Y + 1][Here_X] -= 200;
		}

	}

}

void MAP_output_REKO(void) {
	int length, column;
//表示くそすぎィ！自分、修正いいっすか？
	for (column = 0; column < 16; column++) {
		myprintf("+");
		myprintf("- - -");
	}
	myprintf("+\n");
	for (length = 16; length > 0; length--) {
		myprintf("|     ");
		for (column = 1; column < 16; column++) {
			if (sideY_wall[length][column] >= 32767) {
				myprintf("|");
			} else {
				myprintf(" ");
			}
			myprintf("     ");
		}
		myprintf("|\n");

		myprintf("+");
		for (column = 0; column < 16; column++) {
			if (sideX_wall[length][column] < 32767) {
				myprintf("- - -");
			} else {
				myprintf("     ");
			}
			myprintf("+");
		}
		myprintf("\n");

	}

	for (column = 0; column < 16; column++) {
		myprintf("+");
		myprintf("- - -");
	}
	myprintf("+\n");
}
