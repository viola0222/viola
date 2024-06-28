/*
 * pinsetting.c
 *
 *  Created on: 2017/05/18
 *      Author: 菫子
 */

#include "pinsetting.h"


void pin_setting(void) {
	MPC.PWPR.BIT.B0WI = 0;
	MPC.PWPR.BIT.PFSWE = 1;

//アナログ端子だよ
	MPC.PE4PFS.BIT.ASEL = 1;			//Battery



	MPC.P43PFS.BIT.ASEL = 1;			//ledR
	MPC.PE0PFS.BIT.ASEL = 1;			//ledCR
	MPC.PE2PFS.BIT.ASEL = 1;			//ledC
	MPC.P05PFS.BIT.ASEL = 1;			//ledCL
	MPC.P42PFS.BIT.ASEL = 1;			//ledL

	MPC.P44PFS.BIT.ASEL = 1;			//senR
	MPC.P46PFS.BIT.ASEL = 1;			//senCR
	MPC.PE1PFS.BIT.ASEL = 1;			//senC
	MPC.P40PFS.BIT.ASEL = 1;			//senCL
	MPC.P41PFS.BIT.ASEL = 1;			//senL

	MPC.PE5PFS.BIT.ASEL = 1;			//Gyro

	MPC.PB3PFS.BIT.PSEL = 1;			//PWM_R
	MPC.PB1PFS.BIT.PSEL = 1;			//MTIOC0C
	MPC.PA1PFS.BIT.PSEL = 1;			//PWM_L

//周辺機能だよ
	PORTE.PMR.BIT.B4 = 1;			//ばってりー
	PORTB.PMR.BIT.B3 = 1;			//PWM_R MTIOC0A
	PORTA.PMR.BIT.B1 = 1;			//PWM_L MTIOC0B
	PORTB.PMR.BIT.B1 = 1;			//MTIOC0C

//汎用入出力ポートだよ

	//フォトトラ
	PORT4.PMR.BIT.B0 = 0;			//CL
	PORT4.PMR.BIT.B1 = 0;			//L
	PORT4.PMR.BIT.B4 = 0;			//R
	PORT4.PMR.BIT.B6 = 0;			//CR
	PORTE.PMR.BIT.B1 = 0;			//C

	//ただのLED
	PORTA.PMR.BIT.B3 = 0;
	PORTA.PMR.BIT.B4 = 0;
	PORTA.PMR.BIT.B6 = 0;

	//センサー用LED
	PORT0.PMR.BIT.B5 = 0;			//センサーLED CL
	PORT4.PMR.BIT.B2 = 0;			//L
	PORT4.PMR.BIT.B3 = 0;			//R
	PORTE.PMR.BIT.B0 = 0;			//CR
	PORTE.PMR.BIT.B2 = 0;			//C

	PORT5.PMR.BIT.B5 = 0;			//IN2_L
	PORT5.PMR.BIT.B4 = 0;			//IN1_L
	PORTB.PMR.BIT.B7 = 0;			//IN2_R
	PORTB.PMR.BIT.B6 = 0;			//IN1_R
	PORT3.PMR.BIT.B1 = 0;			//STBY

//入力ポートに設定するよ

	//フォトトラ
	PORT4.PDR.BIT.B0 = 0;			//CL
	PORT4.PDR.BIT.B1 = 0;			//L
	PORT4.PDR.BIT.B4 = 0;			//R
	PORT4.PDR.BIT.B6 = 0;			//CR
	PORTE.PDR.BIT.B1 = 0;			//C

//出力ポートに設定するよ

	PORT0.PDR.BIT.B5 = 1;			//センサーLED CL
	PORT4.PDR.BIT.B2 = 1;			//L
	PORT4.PDR.BIT.B3 = 1;			//R
	PORTE.PDR.BIT.B0 = 1;			//CR
	PORTE.PDR.BIT.B2 = 1;			//C

	PORTA.PDR.BIT.B3 = 1;
	PORTA.PDR.BIT.B4 = 1;
	PORTA.PDR.BIT.B6 = 1;

	PORT5.PDR.BIT.B5 = 1;			//IN2_L
	PORT5.PDR.BIT.B4 = 1;			//IN1_L
	PORTB.PDR.BIT.B7 = 1;			//IN2_R
	PORTB.PDR.BIT.B6 = 1;			//IN1_R
	PORT3.PDR.BIT.B1 = 1;			//STBY

}
