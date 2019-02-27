/*
 * eccdrbg.h
 *
 *  Created on: 2018年8月11日
 *      Author: lzj
 */

#ifndef ECCDRBG_H_
#define ECCDRBG_H_

#include <gmp.h>
#include "sha256.h"

#define ECC_DRBG_MAX_LEN 32

typedef struct {
	mpz_t x;
	mpz_t y;
	mpz_t z;
} eccdrbg_point;

class ECCDRBG {

public:

	ECCDRBG();
	ECCDRBG(const char*c_seed, int base);

	~ECCDRBG();

	void set_a(const char* c_a, int base);
	void set_p(const char* c_p, int base);
	void set_l(const char* c_l, int base);
	void set_seed(const char* c_seed, int base);
	void set_point_P(const char* c_x, const char* c_y, const char* c_z,
			int base);
	void set_point_Q(const char* c_x, const char* c_y, const char* c_z,
			int base);
	void generator(mpz_t &r, eccdrbg_point p_P, eccdrbg_point p_Q);
	void random(int *q, int *p, int M, int m, int N, int n);

	void randomINC(int *q, int *p, int M, int m, int N, int n);

private:
	void ecc_add(eccdrbg_point &R, const eccdrbg_point A,
			const eccdrbg_point B);
	void ecc_double(eccdrbg_point &R, eccdrbg_point A);
	void ecc_mul(eccdrbg_point &R, const mpz_t k, const eccdrbg_point A);
private:
	mpz_t p;
	mpz_t l;
	mpz_t a;
	mpz_t seed;
	eccdrbg_point P;
	eccdrbg_point Q;
	sha256_context hashP;
	sha256_context hashQ;

};

#endif /* ECCDRBG_H_ */
