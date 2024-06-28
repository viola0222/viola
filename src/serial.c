/*
 * serial.c
 *
 *  Created on: 2016/05/04
 *      Author: 駿
 */

#include "iodefine.h"
//注意：iodefine.hの多重インクルードが起こりえます
//インクルードガードをつけるようにしてください
#include <stdio.h>
#include <stdarg.h>

#include "serial.h"

void initSCI(void);

void put1byte(char c);
void putnbyte(char *buf, int len);
int myprintf(const char *fmt, ...);

void initSCI(void) {	//38400 bps

	//低消費電力モードレジスタのプロテクト解除
	SYSTEM.PRCR.WORD = 0xA502;

	//SIC1の低消費電力モード解除
	SYSTEM.MSTPCRB.BIT.MSTPB30 = 0;

	//低消費電力モードレジスタのプロテクト
	SYSTEM.PRCR.WORD = 0xA500;

	SCI1.SCR.BIT.TIE = 0;	//TXI割り込み禁止
	SCI1.SCR.BIT.RIE = 0;	//RXI割り込み禁止

	SCI1.SCR.BIT.TE = 0;	//送信動作禁止
	SCI1.SCR.BIT.RE = 0;	//受信動作禁止

	SCI1.SCR.BIT.TEIE = 0;	//通信終了割り込み禁止

	SCI1.SCR.BYTE = 0x00;

	PORT2.PMR.BIT.B6 = 0; //周辺機能として使う
	PORT3.PMR.BIT.B0 = 0; //周辺機能として使う

	MPC.PWPR.BIT.B0WI = 0; //PFSWEプロテクト解除
	MPC.PWPR.BIT.PFSWE = 1; //PFSのプロテクト解除

	//端子機能レジスタ
	MPC.P26PFS.BIT.PSEL = 10; //P26をTXDに決定
	MPC.P30PFS.BIT.PSEL = 10; //P30をRXDに決定

	MPC.PWPR.BIT.PFSWE = 0; //PFSのプロテクト
	MPC.PWPR.BIT.B0WI = 1; //PFSWEプロテクト

	PORT2.PMR.BIT.B6 = 1; //周辺機能として使う
	PORT3.PMR.BIT.B0 = 1; //周辺機能として使う

	SCI1.SIMR1.BIT.IICM = 0; //シリアルインターフェースモード

	//送信フォーマット
	SCI1.SMR.BIT.CM = 0; //調歩同期式
	SCI1.SMR.BIT.CHR = 0; //データ長8ビット
	SCI1.SMR.BIT.CKS = 0; //PCLK/1
	SCI1.SMR.BIT.PE = 0; //パリティ無し
	SCI1.SMR.BIT.STOP = 0; //STOP機能

	SCI1.SCMR.BIT.SMIF = 0;

	SCI1.SEMR.BIT.ABCS = 0; //16サイクルで転送レート
	//SCI1.SEMR.BIT.NFEN = 0; //ノイズ除去機能

	SCI1.BRR = 40; //80

	SCI1.SCR.BIT.TE = 1;	//送信動作許可
	SCI1.SCR.BIT.RE = 1;	//受信動作許可

}

//送信処理
void put1byte(char c) {

	while ( SCI1.SSR.BIT.TEND == 0)
		;
	SCI1.SSR.BIT.TEND = 0;
	SCI1.TDR = c;

}

void putnbyte(char *buf, int len) {
	int c;
	for (c = 0; c < len; c++) {
		put1byte(buf[c]);
	}
}

int myprintf(const char *fmt, ...) {
	static char buffer[256];
	volatile long len;

	va_list ap;
	va_start(ap, fmt);

	len = vsprintf(buffer, fmt, ap);

	putnbyte(buffer, len);
	va_end(ap);
	return len;
}

/*
 How to use.
 //	init_sci1();
 //	myprintf("%d\n\r", value);
 */
