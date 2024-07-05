/*
 * read_ad.c
 *
 *  Created on: 2017/05/22
 *      Author: 菫子
 */

#include "read_ad.h"

#define Hankei 12.53//12.5474
#define Gearhi 3.33
#define Vattery_Charge 7.0

float Vattery(void) {
	volatile int read;
	volatile float Volt;

	S12AD.ADCSR.BIT.ADST = 0;
	S12AD.ADANS0.BIT.ANS0 = 0x1000;
	S12AD.ADCSR.BIT.ADST = 1;
	while (S12AD.ADCSR.BIT.ADST == 1)
		;

	read = S12AD.ADDR12;
	Volt = ((read * 3.3) / 4096) * 11;

	myprintf("%d %.3f\n", read, Volt);

	if (Volt < Vattery_Charge) {
		myprintf("Please charge the Lipo.\n");
		while (1) {
			PORTA.PODR.BIT.B4 = 1;
			PORTA.PODR.BIT.B6 = 1;
			wait_ms(1000);
			PORTA.PODR.BIT.B4 = 0;
			PORTA.PODR.BIT.B6 = 0;
			wait_ms(1000);
		}
	}

	return Volt;
}

float Encoder(void) {
//volatile int ENCODE_R, ENCODE_L;
	volatile float velo_R, velo_L, velo_G, ENCODE_R, ENCODE_L;

	ENCODE_R = TPU1.TCNT - 32768;
	ENCODE_L = TPU2.TCNT - 32768;
	TPU1.TCNT = 32768;
	TPU2.TCNT = 32768;

	velo_R = ENCODE_R * 1000 * (2 * 3.14 * Hankei / Gearhi / (4096 * 4)); //1000ms(1s),2*Pi*r(mm),4096(digit)*4(A,B,up,down)→ぱるす→,gear12:40=3.3,単位はmm/s
	velo_L = ENCODE_L * 1000 * (2 * 3.14 * Hankei / Gearhi / (4096 * 4));
	velo_G = (velo_R + velo_L) / 2;
//	ENComega = (velo_R - velo_L) / 74;			//反時計回り正

	return velo_G;

}

float EncoderL(void) {
	volatile int ENCODE_R, ENCODE_L;
	volatile float velo_L;

	ENCODE_L = TPU2.TCNT - 32768;
	TPU2.TCNT = 32768;

	velo_L = (float) (ENCODE_L * 1000
			* (2 * 3.14 * Hankei / Gearhi / (4096 * 4)));

	return velo_L;

}

float EncoderR(void) {
	volatile int ENCODE_R, ENCODE_L;
	volatile float velo_R;

	ENCODE_R = TPU1.TCNT - 32768;
	TPU1.TCNT = 32768;

	velo_R = (float) (ENCODE_R * 1000
			* (2 * 3.14 * Hankei / Gearhi / (4096 * 4))); //1000ms(1s),2*Pi*r(mm),4096(digit)*4(A,B,up,down),gear12:40=3.3,mm/s

	return velo_R;

}

int Senser_C(void) {
	volatile int read;
//AN9→0000001000000000 0x200
	S12AD.ADCSR.BIT.ADST = 0;
	S12AD.ADANS0.BIT.ANS0 = 0x200;
	S12AD.ADCSR.BIT.ADST = 1;
	while (S12AD.ADCSR.BIT.ADST == 1)
		;

	read = S12AD.ADDR9;

//	myprintf("%d\n", read);

	return read;
}

int Senser_R(void) {
	volatile int read;
//AN4→000000000010000 0x10
	S12AD.ADCSR.BIT.ADST = 0;
	S12AD.ADANS0.BIT.ANS0 = 0x10;
	S12AD.ADCSR.BIT.ADST = 1;
	while (S12AD.ADCSR.BIT.ADST == 1)
		;

	read = S12AD.ADDR4;

//	myprintf("%d\n", read);

	return read;
}

int Senser_CR(void) {
	volatile int read;
//AN6→0000000001000000 0x40
	S12AD.ADCSR.BIT.ADST = 0;
	S12AD.ADANS0.BIT.ANS0 = 0x40;
	S12AD.ADCSR.BIT.ADST = 1;
	while (S12AD.ADCSR.BIT.ADST == 1)
		;

	read = S12AD.ADDR6;

//	myprintf("%d\n", read);

	return read;
}

int Senser_L(void) {
	volatile int read;
//AN1 -> 0000000000000010
	S12AD.ADCSR.BIT.ADST = 0;
	S12AD.ADANS0.BIT.ANS0 = 0x2;
	S12AD.ADCSR.BIT.ADST = 1;
	while (S12AD.ADCSR.BIT.ADST == 1)
		;

	read = S12AD.ADDR1;

//	myprintf("%d\n", read);

	return read;
}

int Senser_CL(void) {
	volatile int read;
//AN0 -> 0000000000000001
	S12AD.ADCSR.BIT.ADST = 0;
	S12AD.ADANS0.BIT.ANS0 = 0x1;
	S12AD.ADCSR.BIT.ADST = 1;
	while (S12AD.ADCSR.BIT.ADST == 1)
		;

	read = S12AD.ADDR0;

//	myprintf("%d\n", read);

	return read;
}

int Gyro(void) {
	volatile int read, heikin = 0, Volt, dps;

	S12AD.ADCSR.BIT.ADST = 0;
	S12AD.ADANS0.BIT.ANS0 = 0x2000;
	S12AD.ADCSR.BIT.ADST = 1;
	while (S12AD.ADCSR.BIT.ADST == 1)
		;

	read = S12AD.ADDR13;

	return read;
}
