/*
 * lattdrbg.h
 *
 *  Created on: 2018年8月12日
 *      Author: lzj
 */

#ifndef LATTDRBG_H_
#define LATTDRBG_H_

#include "sha256.h"
#include <gmp.h>
#include <stddef.h>
#include <string>
#include <cstring>

class LATTDRBG {

public:
	LATTDRBG();
	~LATTDRBG();

	void initMatrixA();
	void set_m1(int m1);
	void set_m2(int m2);
	void set_n(int n);
	void set_k(int k);
	void set_q(int q);
	void set_seed(unsigned char* seed[32]);
	void set_seedHexStr(const char * seed);
	void random(int *p, int *q, int M, int m, int N, int n);

private:
	void hexStrToByte(const char* source, unsigned char* dest, int sourceLen);

private:
	int ** _A;
	int __m1;
	int __m2;
	int __m;
	int __q;
	int __n;
	int __k;
	unsigned char __seed[32];
	sha256_context __hash;

};

#endif /* LATTDRBG_H_ */
