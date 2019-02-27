/*
 * main.c
 *
 *  Created on: 2018年8月1日
 *      Author: lzj
 */

#include <stdio.h>

#include "sha256.h"
#include "eccdrbg.h"
#include "timing.h"
#include "lattdrbg.h"
#include <time.h>
#include <stdio.h>
#include <iostream>
using namespace std;
#include  <stdlib.h>
#include  <time.h>

static void byte_to_hex(uint8_t b, char s[23]) {
	unsigned i = 1;
	s[0] = s[1] = '0';
	s[2] = '\0';
	while (b) {
		unsigned t = b & 0x0f;
		if (t < 10) {
			s[i] = '0' + t;
		} else {
			s[i] = 'a' + t - 10;
		}
		i--;
		b >>= 4;
	}
}

void ecc_test() {
	int q[1000] = { 0 };
	int p[1000] = { 0 };

	ECCDRBG ecc;

	ecc.set_point_P(
			"6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296",
			"4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5",
			"1", 16);
	ecc.set_point_Q(
			"c97445f45cdef9f0d3e05e1e585fc297235b82b5be83efca67c59852018192",
			"b28ef557ba31dfcbdd21ac46e2a91e3c304f44cb87058ada2cb815151e610046",
			"1", 16);

	ecc.set_a("-3", 10);
	ecc.set_l(
			"115792089210356248762697446949407573529996955224135760342422259061068512044369",
			16);
	ecc.set_p(
			"115792089210356248762697446949407573530086143415290314195533631308867097853951",
			16);

	ecc.set_seed(
			"115792089210356248762697446949407573529996955224135760342422259061068512044222",
			16);

//	int start = cputime();
//	ecc.random(q, p, 100, 100, 100, 100);
//	int end = cputime();
//
//	printf("%d  \n", end - start);

//	start = cputime();
	ecc.randomINC(q, p, 100, 100, 100, 100);
//	end = cputime();

//	printf("inc %d  \n", end - start);
	for (int i = 0; i < 100; ++i) {
		printf("%d : %d  \n", p[i], q[i]);
	}

}



void lat_test(){
	LATTDRBG la;

		int p[100];
		int q[100];

		la.set_k(78);
		la.set_m1(48);
		la.set_m2(19968);
		la.set_n(128);
		la.set_q(2147483647);
		la.set_seedHexStr("115792089210356248762697446949407573529996955224135760342422259061068512044222");
		la.initMatrixA();

		la.random(p,q,100,100,100,100);

		for (int i = 0; i < 100; ++i) {

			cout<<p[i]<<"  ";
			cout<<q[i]<<endl;

		}

}
int main() {

//	ecc_test();

lat_test();

	/**
	 * for(int i = 0; i < m; ++i）

      delete []a[i];

delete []a;
	 */
	return 0;
}
