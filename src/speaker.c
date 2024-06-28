/*
 * speaker.c
 *
 *  Created on: 2017/10/17
 *      Author: 菫子
 */
#include "speaker.h"

void init_TPU4(void) { //スピーカー用
	SYSTEM.PRCR.WORD = 0xA502;	//レジスタプロテクション解除
	SYSTEM.MSTPCRA.BIT.MSTPA13 = 0;			//スタンバイ解除

	MPC.PWPR.BIT.B0WI = 0; //PFSWEプロテクト解除
	MPC.PWPR.BIT.PFSWE = 1; //PFSのプロテクト解除 書き込み許可
	MPC.PB5PFS.BIT.PSEL = 0x4;			//位相係数モード

	MPC.PWPR.BIT.B0WI = 1; //PFSWEプロテクト
	MPC.PWPR.BIT.PFSWE = 0; //PFSのプロテクト　書き込み禁止

	PORTB.PMR.BIT.B5 = 1;	//周辺機能に設定

	TPU4.TCR.BIT.TPSC = 0x1;	//カウンタクロックの設定 PCLK/4
	TPU4.TCR.BIT.CKEG = 0x0;	//入力クロックのエッジ 立ち下がり
	TPU4.TCR.BIT.CCLR = 0x2;	//カウンタクリア要因の設定
	TPU4.TIOR.BIT.IOA = 0;

	TPU1.TCNT = 20;
	TPU2.TCNT = 20;

	TPU4.TMDR.BIT.MD = 0x3;		//PWMモード2

	TPUA.TSTR.BIT.CST1 = 1;
	TPUA.TSTR.BIT.CST2 = 1;

}
