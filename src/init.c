/*
 * init.c
 *
 *  Created on: 2017/05/22
 *      Author: 菫子
 */

#include "init.h"

void init_CMT(void) {
	SYSTEM.PRCR.WORD = 0xA502;	//レジスタプロテクション解除
	MSTP(CMT0) = 0;				//消費電力軽減設定解除
	CMT.CMSTR0.BIT.STR0 = 0;	//ステータスレジスタ
	CMT0.CMCR.BIT.CKS = 0;		//クロック
	CMT0.CMCOR = 6250 - 1;		//割り込み周期 1ms:6250
	CMT0.CMCR.BIT.CMIE = 1;		//コンペアマッチ割り込みイネーブル
	IEN(CMT0,CMI0)=1;
	IPR(CMT0,CMI0)=1;			//割り込みのベクターテーブル
	CMT.CMSTR0.BIT.STR0 = 1;	//ステータスレジスタ
}

void init_AD(void) {
	SYSTEM.PRCR.WORD = 0xA502;	//レジスタプロテクション解除
	MSTP(S12AD) = 0;			//消費電力軽減設定解除
	S12AD.ADCSR.BIT.ADST = 0;	//AD変換停止
	S12AD.ADCSR.BIT.EXTRG = 1;	//非同期トリガに設定
	S12AD.ADCSR.BIT.TRGE = 0;	//AD変換の開始の禁止
	S12AD.ADCSR.BIT.ADIE = 1;	//スキャン終了後のS12ADI0割り込み発生の許可
	S12AD.ADCSR.BIT.ADCS = 0;	//スキャンモードをシングルスキャンモードに設定
	S12AD.ADCSR.BIT.CKS = 0;	//AD変換のクロックをPCLK/8に設定
	//S12AD.ADCSR.BIT.ADST = 1;
}
//
//void init_SCI(void) {
//
//	SYSTEM.PRCR.WORD = 0xA502;	//レジスタプロテクション解除
//	MPC.PWPR.BIT.B0WI = 0;
//	MPC.PWPR.BIT.PFSWE = 1;
//
//	SYSTEM.MSTPCRB.BIT.MSTPB30 = 0;			//スタンバイ解除
////シリアルポートを設定
//	PORT2.PMR.BIT.B6 = 1;
//	PORT3.PMR.BIT.B0 = 1;
//	MPC.P26PFS.BIT.PSEL = 0x0A;
//	MPC.P30PFS.BIT.PSEL = 0x0A;
//	SCI1.SMR.BYTE = 0x08;
//	SCI1.SCMR.BYTE = 0xF2;
//	SCI1.SEMR.BYTE = 0x00;
//	SCI1.SCR.BYTE = 0x08;		//送受信割り込み禁止
//	SCI1.SEMR.BIT.ABCS = 0;
//	SCI1.SMR.BIT.MP = 1;
//	SCI1.SMR.BYTE = 0x0;
//	SCI1.BRR = 13;			//ビットレート設定115200
//	SCI1.SCR.BIT.TE = 1;		//送信許可
//	SCI1.SCR.BIT.RE = 1;		//受信許可
//}

void init_TPU(void) {
	SYSTEM.PRCR.WORD = 0xA502;	//レジスタプロテクション解除
	SYSTEM.MSTPCRA.BIT.MSTPA13 = 0;			//スタンバイ解除

	MPC.PWPR.BIT.B0WI = 0; //PFSWEプロテクト解除
	MPC.PWPR.BIT.PFSWE = 1; //PFSのプロテクト解除
	MPC.P14PFS.BIT.PSEL = 0x4;			//位相係数モード
	MPC.P15PFS.BIT.PSEL = 0x4;			//位相係数モード
	MPC.P16PFS.BIT.PSEL = 0x4;			//位相係数モード
	MPC.P17PFS.BIT.PSEL = 0x4;			//位相係数モード
	MPC.PWPR.BIT.B0WI = 1; //PFSWEプロテクト
	MPC.PWPR.BIT.PFSWE = 0; //PFSのプロテクト
	PORT1.PMR.BIT.B4 = 1;
	PORT1.PMR.BIT.B5 = 1;
	PORT1.PMR.BIT.B6 = 1;
	PORT1.PMR.BIT.B7 = 1;

	TPU1.TCNT = 20;
	TPU2.TCNT = 20;

	TPU1.TMDR.BIT.MD = 4;
	TPU2.TMDR.BIT.MD = 4;
	TPUA.TSTR.BIT.CST1 = 1;
	TPUA.TSTR.BIT.CST2 = 1;

}


void init_MTU(void) {
	SYSTEM.PRCR.WORD = 0xA502;	//レジスタプロテクション解除
	SYSTEM.MSTPCRA.BIT.MSTPA9 = 0;			//スタンバイ解除

	MTU0.TCR.BIT.TPSC = 0;			//1分周
	MTU0.TCR.BIT.CKEG = 0;			//立ち上がりエッジでカウント
	MTU0.TCR.BIT.CCLR = 5;			//TGRCのコンペアマッチでTCNTをクリア(TGRCで周期決める）

	MTU0.TMDR.BIT.MD = 3;			//PWMモード2:3を選択

	MTU0.TIORH.BIT.IOA = 5;			//High,コンペアマッチでLow
	MTU0.TIORH.BIT.IOB = 5;			//High,コンペアマッチでLow
	MTU0.TIORL.BIT.IOC = 2;			//Low,コンペアマッチでHigh
	MTU0.TGRA = 20;			//右
	MTU0.TGRB = 20;			//左
	MTU0.TGRC = 250;			//コンペアマッチクリア 512 変えない
	MTU.TSTR.BIT.CST0 = 1;

}
