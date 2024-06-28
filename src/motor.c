/*
 * motor.c
 *
 *  Created on: 2017/05/18
 *      Author: 菫子
 */

#include "motor.h"

extern volatile int DkFlag, Dk_omega_Flag, Dk_omega_minas_Flag, Dk_END_Flag,
		Slalom_Flag, turn_R_Flag, turn_L_Flag;
extern volatile float Gyroomega, Gyrosita;
extern volatile float Vg, Xg;
extern volatile float target_V, target_X, target_Accel, target_omega_V,
		target_omega_X, target_omega_Accel;

//ログ取り用
extern volatile int count_time, Log_Flag, failsafe_count, FailSafe_Flag;

extern volatile float ErrorSenser_P, ControlSenserG, ErrorG_P, ErrorG_I,
		ControlG, ErroromegaG_P, ErroromegaG_I, ControlomegaG, ErrorSenser_D;

void Error_Reset(void) {
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

	//エンコーダー初期化
	TPU1.TCNT = 32768;
	TPU2.TCNT = 32768;
}

float abs(float number) {

	if (number < 0) {
		number = -number;
	}

	return number;
}

void motor_off(void) {
	MTU.TSTR.BIT.CST0 = 0;
	PORT5.PODR.BIT.B5 = 0;			//IN2_L
	PORT5.PODR.BIT.B4 = 0;			//IN1_L
	PORTB.PODR.BIT.B7 = 0;			//IN2_R
	PORTB.PODR.BIT.B6 = 0;			//IN1_R
	MTU.TSTR.BIT.CST0 = 1;
	Log_Flag = 0;
}

void Hasiru(volatile int dutyR, volatile int dutyL) {
	MTU.TSTR.BIT.CST0 = 0;

	if (dutyR < 0) {
		//モーター逆回転
		dutyR = -dutyR;
		if (dutyR > 200) {
			dutyR = 200;
		}
		PORTB.PODR.BIT.B7 = 1;			//IN2_R
		PORTB.PODR.BIT.B6 = 0;			//IN1_R
	} else {
		if (dutyR > 200) {
			dutyR = 200;
		}
		//モーター正回転
		PORTB.PODR.BIT.B7 = 0;			//IN2_R
		PORTB.PODR.BIT.B6 = 1;			//IN1_R
	}

	MTU0.TGRA = dutyR;

	if (dutyL < 0) {
		//モーター逆回転
		dutyL = -dutyL;
		if (dutyL > 200) {
			dutyL = 200;
		}
		PORT5.PODR.BIT.B5 = 1;			//IN2_L
		PORT5.PODR.BIT.B4 = 0;			//IN1_L
	} else {
		if (dutyL > 200) {
			dutyL = 200;
		}
		//モーター正回転
		PORT5.PODR.BIT.B5 = 0;			//IN2_L
		PORT5.PODR.BIT.B4 = 1;			//IN1_L
	}

	MTU0.TGRB = dutyL;

	MTU.TSTR.BIT.CST0 = 1;
}

int Linear_motion(volatile float X_all, volatile float Acceleration,
		volatile float V_max, volatile float V_first, volatile float V_last) {

	volatile float Xpkasoku, Xmkasoku, X_const;
	volatile int i = 0;
	volatile float time_acceleration, time_const, time_deceleration;
	int Sankaku_Flag = 0;

	if (FailSafe_Flag == 1) {
		motor_off();
		return 0;
	}
	/*
	 * volatile float X_all,目標走行距離
	 * volatile float Acceleration,加速度
	 volatile float V_max, 台形加速の速度一定値の時の目標速度
	 volatile float V_first,台形加速始めた時点の目標速度
	 volatile float V_last台形加速終わった時の目標速度
	 * */

	Xpkasoku = abs((V_max * V_max - V_first * V_first) / (2 * Acceleration));//加速中に進む距離
	Xmkasoku = abs((V_max * V_max - V_last * V_last) / (2 * Acceleration));	//減速中に進む距離
	X_const = X_all - Xpkasoku - Xmkasoku;			//一定速度の間に進む距離

	/*加速にかかる時間 */
	time_acceleration = (abs(V_max) - abs(V_first)) * 1000 / abs(Acceleration);

	/*減速にかかる時間*/
	time_deceleration = (abs(V_max) - abs(V_last)) * 1000 / abs(Acceleration);

	/*加速中に進む距離*/
	Xpkasoku = abs((V_max * V_max - V_first * V_first) / (2 * Acceleration));

	/*減速中に進む距離*/
	Xmkasoku = abs((V_max * V_max - V_last * V_last) / (2 * Acceleration));
	/*一定速度の間に進む距離*/
	X_const = X_all - Xpkasoku - Xmkasoku;

	/*速度一定の区間を走る時間*/
	time_const = X_const * 1000 / abs(V_max);

	if (X_const < 0) {
		//三角加速なので例外処理

		Xpkasoku = X_all
				+ abs(
						(V_first * V_first + V_last * V_last)
								/ (2 * Acceleration));
		Xmkasoku = X_all - Xpkasoku;

		time_const = 0;
		Sankaku_Flag = 1;
	}

	Error_Reset();

	target_X = 0;
	target_V = V_first;
	target_Accel = Acceleration;

	target_omega_X = 0;
	target_omega_V = 0;
	target_omega_Accel = 0;

	Xg = 0;

	Dk_END_Flag = 0;
	DkFlag = 1;
	count_time = 0;

	while (1) {
		if ((ErrorG_I > 1200000) && (FailSafe_Flag == 1)) {
			while (PORT3.PIDR.BIT.B5 == 0) {
				DkFlag = 0;
				motor_off();

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

			}

			return 0;
			//fail safe
		}

		/*台形加速 加速区間*/
		if (count_time < (int) time_acceleration) {
			target_Accel = Acceleration;
		}

		/*速度一定*/
		else if ((count_time >= (int) time_acceleration)
				&& (count_time < (int) (time_acceleration + time_const))) {
			target_Accel = 0;
			PORTA.PODR.BIT.B3 = 0;
			PORTA.PODR.BIT.B4 = 0;
			PORTA.PODR.BIT.B6 = 0;

			if (Sankaku_Flag == 0) {
				target_V = V_max;
			} else {
				/*三角加速の時は何もしない*/
			}
		}

		/*台形加速 減速区間*/
		else if (count_time
				< (int) (time_acceleration + time_const + time_deceleration)) {
			target_Accel = -Acceleration;
//			PORTA.PODR.BIT.B3 = 1;
//			PORTA.PODR.BIT.B4 = 0;
//			PORTA.PODR.BIT.B6 = 0;
		}

		if (count_time
				>= (int) (time_acceleration + time_const + time_deceleration)) {
			/*全部走り切ったってさ*/
			target_Accel = 0;

			if (abs(Xg) < X_all) {
				//距離足りなかったら現在の速度保ってほしい
//				PORTA.PODR.BIT.B3 = 1;
//				PORTA.PODR.BIT.B4 = 1;
//				PORTA.PODR.BIT.B6 = 1;
				if (V_last != 0) {
					//終端速度があるなら勝手に走ってくれると思うので何もしない.

				} else if (Acceleration < 0) {
					/*加速度負の時で終端速度がゼロの時*/
					target_V = -10;
				} else {
					/*加速度正の時で終端速度がゼロの時*/
					target_V = 10;
				}
			} else {
				//ちゃんと距離分はしってたら
				target_V = V_last;
				//終端速度に追従させるよ
				if (V_last == 0) {
					Dk_END_Flag = 1;
					wait_ms(100);
				}
				ErrorG_I = 0;
				Dk_END_Flag = 0;
				Sankaku_Flag = 0;
				DkFlag = 0;
//				PORTA.PODR.BIT.B3 = 1;
//				PORTA.PODR.BIT.B4 = 0;
//				PORTA.PODR.BIT.B6 = 1;
				return 0;
			}
		}
	}
	return 0;
}

void Ultra_pivot_turn(volatile float Xrad_all, volatile float Kaku_Acceleration,
		volatile float V_omega_max, volatile float V_omega_first,
		volatile float V_omega_last) {

	volatile float Xp_omega_kasoku, Xm_omega_kasoku, X_omega_const;
	volatile float time_acceleration, time_const, time_deceleration;
	/*
	 * volatile float Xrad_all,目標走行距離 (何度曲がりたいか？)
	 * volatile float Kaku_Acceleration,角加速度
	 volatile float V_omega_max, 超新地旋回中の速度一定値の時の目標速度
	 volatile float V_omega_first,超新地旋回始めた時点の目標速度
	 volatile float V_omega_last:超新地旋回終わった時の目標速度
	 * */

	if (FailSafe_Flag == 1) {
		return;
	}

	/*加速にかかる時間 */
	time_acceleration = (abs(V_omega_max) - abs(V_omega_first)) * 1000
			/ abs(Kaku_Acceleration);

	/*減速にかかる時間*/
	time_deceleration = (abs(V_omega_max) - abs(V_omega_last)) * 1000
			/ abs(Kaku_Acceleration);

	/*加速中に進む距離*/
	Xp_omega_kasoku = abs(
			(V_omega_max * V_omega_max - V_omega_first * V_omega_first)
					/ (2 * Kaku_Acceleration));
	/*減速中に進む距離*/
	Xm_omega_kasoku = abs(
			(V_omega_max * V_omega_max - V_omega_last * V_omega_last)
					/ (2 * Kaku_Acceleration));
	/*一定速度の間に進む距離*/
	X_omega_const = Xrad_all - Xp_omega_kasoku - Xm_omega_kasoku;

	/*速度一定の区間を走る時間*/
	time_const = X_omega_const * 1000 / abs(V_omega_max);

	Dk_omega_minas_Flag = 0;
	Error_Reset();

	Gyrosita = 0;
	target_X = 0;
	target_V = 0;
	target_Accel = 0;

	target_omega_X = 0;
	target_omega_V = V_omega_first;
	target_omega_Accel = Kaku_Acceleration;

	if (Kaku_Acceleration < 0) {
		Dk_omega_minas_Flag = 1;
	}
	Dk_omega_Flag = 1;
	count_time = 0;

	while (1) {

		if (abs(Gyroomega) > 3 * abs(V_omega_max)) {
			while (PORT3.PIDR.BIT.B5 == 0) {
				Dk_omega_Flag = 0;
				motor_off();
			}
			//fail safe
		}

		/*台形加速はっじまっるよー！*/
		if (count_time < (int) time_acceleration) {
			target_omega_Accel = Kaku_Acceleration;
		}

		/*速度一定にするお*/
		else if ((count_time >= (int) time_acceleration)
				&& (count_time < (int) (time_acceleration + time_const))) {
			target_omega_Accel = 0;
			target_omega_V = V_omega_max;
		}

		/*減速するお*/
		else if (count_time
				< (int) (time_acceleration + time_const + time_deceleration)) {
			target_omega_Accel = -Kaku_Acceleration;
		}

		if (count_time
				>= (int) (time_acceleration + time_const + time_deceleration)) {
			target_omega_Accel = 0;
			if (abs(Gyrosita) < Xrad_all) {
				PORTA.PODR.BIT.B3 = 1;
				PORTA.PODR.BIT.B4 = 1;
				PORTA.PODR.BIT.B6 = 1;
				if (V_omega_max > 0) {
					target_omega_V = 10;
				} else {
					target_omega_V = -10;
				}

			} else {
				//走り切ったらおしまい
				target_omega_V = V_omega_last;
				wait_ms(100);
				PORTA.PODR.BIT.B3 = 0;
				PORTA.PODR.BIT.B4 = 0;
				PORTA.PODR.BIT.B6 = 0;
				ErroromegaG_I = 0;
				Dk_omega_minas_Flag = 0;
				Dk_omega_Flag = 0;
				break;
			}
		}
	}
}

void Slalom(volatile float X_slalom_rad_all, volatile float Kaku_Acceleration,
		volatile float V_slalom_max, volatile float V_slalom_const) {

	volatile float Xp_slalom_kasoku, Xm_slalom_kasoku, X_slalom_const;
	/*正->反時計方向
	 * volatile float X_slalom_rad_all, 90ど
	 *目標走行距離 (90mm * 3.14 * theta) / 360;	回転距離＝回転したい角度*π*回転したい距離/360°
	 * volatile float Kaku_Acceleration,加速度
	 volatile float V_slalom_max, スラロームの円弧の時(一定速)の目標角速度
	 volatile float V_slalom_const,スラローム中の目標速度（重心速度）
	 volatile float V_slalom_omega:スラローム終わった時の目標角速度（スラロームから台形加速はいるときの速度） どうせ0
	 * */

	if (FailSafe_Flag == 1) {
		return 0;
	}

	turn_L_Flag = 1;
	Gyrosita = 0;

	Xp_slalom_kasoku = abs(
			(V_slalom_max * V_slalom_max) / (2 * Kaku_Acceleration));//加速クロソイド区間に進む距離(角度)
	Xm_slalom_kasoku = abs(
			(V_slalom_max * V_slalom_max) / (2 * Kaku_Acceleration));//減速クロソイド区間に進む距離(角度)
	X_slalom_const = abs(
			X_slalom_rad_all - Xp_slalom_kasoku - Xm_slalom_kasoku);//一定速度の間に進む距離(角度)

	Error_Reset();

	target_X = 0;
	target_V = V_slalom_const;
	target_Accel = 0;

	target_omega_X = 0;
	target_omega_V = 0;
	target_omega_Accel = 0;

	if (Kaku_Acceleration < 0) {
		turn_R_Flag = 1;
	}

	count_time = 0;
	Slalom_Flag = 1;

	while (1) {
		if (abs(Gyroomega) > 3 * abs(V_slalom_max)) {
			while (PORT3.PIDR.BIT.B5 == 0) {
				motor_off();
			}
			//fail safe
		}

		/*スラローム角速度加速区間*/
		if (abs(target_omega_X) < Xp_slalom_kasoku) {
			target_omega_Accel = Kaku_Acceleration;
		}

		/*角速度一定*/
		else if ((abs(target_omega_X) > Xp_slalom_kasoku)
				&& (abs(target_omega_X) <= (X_slalom_rad_all - Xm_slalom_kasoku))) {
			target_omega_Accel = 0;
			target_omega_V = V_slalom_max;
		}

		/*スラローム角速度減速区間*/
		else if (abs(target_omega_X) <= X_slalom_rad_all) {
			target_omega_Accel = -Kaku_Acceleration;
		}
		/*スラロームおわり*/
		if (abs(target_omega_X) > X_slalom_rad_all) {
			target_omega_Accel = 0;
			target_omega_V = 0;
			ErrorG_I = 0;
			ErroromegaG_I = 0;

			turn_R_Flag = 0;
			turn_L_Flag = 0;
			Slalom_Flag = 0;
			break;
		}
	}
}

void Ultra_pivot_Slalom(volatile float X_slalom_rad_all,
		volatile float Kaku_Acceleration, volatile float V_omega_max,
		volatile float Vg_const) {

	volatile float Xp_slalom_kasoku, Xm_slalom_kasoku, X_slalom_const;
	volatile float time_acceleration, time_const, time_deceleration;
	/*正->反時計方向
	 * volatile float X_slalom_rad_all, 90ど
	 *目標走行距離 (90mm * 3.14 * theta) / 360;	回転距離＝回転したい角度*π*回転したい距離/360°
	 * volatile float Kaku_Acceleration,加速度
	 volatile float V_omega_max, スラロームの円弧の時(一定速)の目標角速度
	 volatile float Vg_const,スラローム中の目標速度（重心速度）
	 * */

	if (FailSafe_Flag == 1) {
		motor_off();
		return;
	}
	Slalom_Flag = 1;
	turn_L_Flag = 1;
	Gyrosita = 0;

	/*加速にかかる時間 */
	time_acceleration = abs(V_omega_max) * 1000 / abs(Kaku_Acceleration);

	/*減速にかかる時間*/
	time_deceleration = time_acceleration;

	Xp_slalom_kasoku = abs(
			(V_omega_max * V_omega_max) / (2 * Kaku_Acceleration));	//加速クロソイド区間に進む距離(角度)
	Xm_slalom_kasoku = abs(
			(V_omega_max * V_omega_max) / (2 * Kaku_Acceleration));	//減速クロソイド区間に進む距離(角度)
	X_slalom_const = abs(
			X_slalom_rad_all - Xp_slalom_kasoku - Xm_slalom_kasoku);//一定速度の間に進む距離(角度)

	time_const = X_slalom_const * 1000 / abs(V_omega_max);

	Error_Reset();

	target_X = 0;
	target_V = Vg_const;
	target_Accel = 0;

	target_omega_X = 0;
	target_omega_V = 0;
	target_omega_Accel = Kaku_Acceleration;

	if (Kaku_Acceleration < 0) {
		turn_R_Flag = 1;
	}

	count_time = 0;

	while (1) {
		if (abs(Gyroomega) > 3 * abs(V_omega_max)) {
			while (PORT3.PIDR.BIT.B5 == 0) {
				motor_off();
			}
			//fail safe
		}

		/*スラローム角速度加速区間*/
		if (count_time < (int) time_acceleration) {
			target_omega_Accel = Kaku_Acceleration;
		}

		/*角速度一定*/
		else if ((count_time >= (int) time_acceleration)
				&& (count_time < (int) (time_acceleration + time_const))) {
			target_omega_Accel = 0;
			target_omega_V = V_omega_max;
		}

		/*スラローム角速度減速区間*/
		else if (count_time
				< (int) (time_acceleration + time_const + time_deceleration)) {
			target_omega_Accel = -Kaku_Acceleration;
		}
		/*スラロームおわり*/
		if (count_time
				>= (int) (time_acceleration + time_const + time_deceleration)) {
			target_omega_Accel = 0;
			if (abs(Gyrosita) < X_slalom_rad_all) {

			} else {
				target_omega_V = 0;
				ErrorG_I = 0;
				ErroromegaG_I = 0;

				turn_R_Flag = 0;
				turn_L_Flag = 0;
				Slalom_Flag = 0;
				break;
			}
		}
	}
}

