/*
 * eccdrbg.cpp
 *
 *  Created on: 2018年8月11日
 *      Author: lzj
 */

#include "eccdrbg.h"

ECCDRBG::ECCDRBG() {
	mpz_inits(p, l, a, seed, P.x, P.y, P.z, Q.x, Q.y, Q.z, NULL);
	sha256_init(&hashP);
	sha256_init(&hashQ);
}

ECCDRBG::ECCDRBG(const char*c_seed, int base) {
	mpz_inits(p, l, a, P.x, P.y, P.z, Q.x, Q.y, Q.z, NULL);
	mpz_init_set_str(this->seed, c_seed, base);
	sha256_init(&hashP);
	sha256_init(&hashQ);
}

ECCDRBG::~ECCDRBG() {
	mpz_clears(p, l, a, seed, P.x, P.y, P.z, Q.x, Q.y, Q.z, NULL);
	sha256_free(&hashP);
	sha256_free(&hashQ);
}

void ECCDRBG::ecc_add(eccdrbg_point &R, const eccdrbg_point A,
		const eccdrbg_point B) {

	mpz_t u, v, aA, t1, t2, t3, t4, t5, t6, t7, t8;

	mpz_inits(u, v, aA, t1, t2, t3, t4, t5, t6, t7, t8, NULL);

	//t1 = y1z2
	mpz_mul(t1, A.y, B.z);

	//t2 = y2z1
	mpz_mul(t2, B.y, A.z);

	//t3 = x1z2
	mpz_mul(t3, A.x, B.z);

	//t4 = x2z1
	mpz_mul(t4, B.x, A.z);

	//t5=z1z2
	mpz_mul(t5, B.z, A.z);

	//u = t2-t1
	mpz_sub(u, t2, t1);

	//v = t4-t3
	mpz_sub(v, t4, t3);

	//t6 = u^2
	mpz_pow_ui(t6, u, 2);

	//t7 = v^2
	mpz_pow_ui(t7, v, 2);

	//t8=v^3
	mpz_mul(t8, t7, v);

	//A
	mpz_mul(aA, t6, t5);
	mpz_sub(aA, aA, t8);
	mpz_mul_ui(t2, t7, 2);
	mpz_submul(aA, t2, t3);

	//x3
	mpz_mul(R.x, v, aA);
	mpz_mod(R.x, R.x, p);

	//y3
	mpz_mul(t2, t7, t3);
	mpz_sub(t2, t2, aA);
	mpz_mul(R.y, t2, u);
	mpz_submul(R.y, t8, t1);
	mpz_mod(R.y, R.y, p);

	//z3
	mpz_mul(R.z, t8, t5);
	mpz_mod(R.z, R.z, p);

	mpz_clears(u, v, aA, t1, t2, t3, t4, t5, t6, t7, t8, NULL);

}
void ECCDRBG::ecc_double(eccdrbg_point &R, eccdrbg_point A) {
	mpz_t w, s, b, h, t1, t2, t3;

	mpz_inits(w, s, b, h, t1, t2, t3, NULL);

	//w
	mpz_pow_ui(w, A.x, 2);
	mpz_mul_ui(w, w, 3);
	mpz_pow_ui(t1, A.z, 2);
	mpz_addmul(w, t1, a);

	//s
	mpz_mul(s, A.z, A.y);

	//b
	mpz_mul(b, A.x, A.y);
	mpz_mul(b, b, s);

	//h
	mpz_pow_ui(h, w, 2);
	mpz_submul_ui(h, b, 8);

	//x3
	mpz_mul_ui(R.x, h, 2);
	mpz_mul(R.x, R.x, s);
	mpz_mod(R.x, R.x, p);

	//y3
	mpz_mul_ui(t1, b, 4);
	mpz_sub(t1, t1, h);
	mpz_mul(t1, t1, w);
	mpz_pow_ui(t3, A.y, 2);
	mpz_pow_ui(t2, s, 2);
	mpz_mul(t3, t3, t2);
	mpz_submul_ui(t1, t3, 8);
	mpz_mod(R.y, t1, p);

	//z3
	mpz_mul(R.z, t2, s);
	mpz_mul_ui(R.z, R.z, 8);
	mpz_mod(R.z, R.z, p);

	mpz_clears(w, s, b, h, t1, t2, t3, NULL);

}
void ECCDRBG::ecc_mul(eccdrbg_point &R, const mpz_t k, const eccdrbg_point A) {

	int i;

	int len = mpz_sizeinbase(k, 2);

	for (i = 0; i < len; ++i) {
		if (mpz_tstbit(k, i)) {
			mpz_set(R.x, A.x);
			mpz_set(R.y, A.y);
			mpz_set(R.z, A.z);
			break;
		}
	}

	for (; i < len; ++i) {
		ecc_double(R, R);
		if (mpz_tstbit(k, i)) {
			ecc_add(R, R, A);
		}
	}

}

void ECCDRBG::set_a(const char* c_a, int base) {
	mpz_set_str(a, c_a, base);

}
void ECCDRBG::set_p(const char* c_p, int base) {
	mpz_set_str(p, c_p, base);

}
void ECCDRBG::set_l(const char* c_l, int base) {
	mpz_set_str(l, c_l, base);
}
void ECCDRBG::set_seed(const char* c_seed, int base) {
	mpz_set_str(seed, c_seed, base);

}
void ECCDRBG::set_point_P(const char* c_x, const char* c_y, const char* c_z,
		int base) {
	mpz_set_str(P.x, c_x, base);
	mpz_set_str(P.y, c_y, base);
	mpz_set_str(P.z, c_z, base);

}
void ECCDRBG::set_point_Q(const char* c_x, const char* c_y, const char* c_z,
		int base) {
	mpz_set_str(Q.x, c_x, base);
	mpz_set_str(Q.y, c_y, base);
	mpz_set_str(Q.z, c_z, base);

}
void ECCDRBG::generator(mpz_t &r, eccdrbg_point p_P, eccdrbg_point p_Q) {

	eccdrbg_point T;

	mpz_inits(T.x, T.y, T.z, NULL);
	ecc_mul(T, seed, p_P);
	mpz_set(seed, T.x);
	ecc_mul(T, seed, p_Q);
	mpz_set(r, T.x);
	mpz_clears(T.x, T.y, T.z, NULL);

}
void ECCDRBG::random(int *q, int *p, int M, int m, int N, int n) {
	eccdrbg_point p_P;
	eccdrbg_point p_Q;
	mpz_t r, tmp;
	int count;
	size_t len_seed;
	unsigned char *buf_seed;
	unsigned char out[ECC_DRBG_MAX_LEN];

	mpz_inits(r, p_P.x, p_P.y, p_P.z, p_Q.x, p_Q.y, p_Q.z, NULL);

	buf_seed = (unsigned char *) mpz_limbs_read(seed);
	len_seed = mpz_size(seed) * sizeof(mp_limb_t) / sizeof(unsigned char);

	sha256_starts(&hashP, 0);
	sha256_update(&hashP, buf_seed, len_seed);
	sha256_update(&hashP, (const unsigned char *) &M, sizeof(unsigned int));
	sha256_update(&hashP, (const unsigned char *) &N, sizeof(unsigned int));
	sha256_update(&hashP, (const unsigned char *) &m, sizeof(unsigned int));
	sha256_update(&hashP, (const unsigned char *) &n, sizeof(unsigned int));
	sha256_clone(&hashQ, &hashP);

	//P
	sha256_update(&hashP, (unsigned char *) mpz_limbs_read(P.x),
			(size_t) (mpz_size(P.x) * sizeof(mp_limb_t) / sizeof(unsigned char)));
	sha256_finish(&hashP, out);

	mpz_roinit_n(tmp, (mp_limb_t *) out,
			(mp_size_t) (ECC_DRBG_MAX_LEN * sizeof(unsigned char)
					/ sizeof(mp_limb_t)));
	mpz_set(r, tmp);
	ecc_mul(p_P, r, P);

	//Q
	sha256_update(&hashQ, (unsigned char *) mpz_limbs_read(Q.x),
			(size_t) (mpz_size(Q.x) * sizeof(mp_limb_t) / sizeof(unsigned char)));
	sha256_finish(&hashQ, out);

	mpz_roinit_n(tmp, (mp_limb_t *) out,
			(mp_size_t) (ECC_DRBG_MAX_LEN * sizeof(unsigned char)
					/ sizeof(mp_limb_t)));
	mpz_set(r, tmp);
	ecc_mul(p_Q, r, Q);

	count = 0;
	while (count < m) {
		generator(r, p_P, p_Q);
		mpz_mod_ui(r, r, M);
		int m0 = mpz_get_ui(r);
		int i;
		for (i = 0; i < count; ++i) {
			if (p[i] == m0) {
				break;
			}
		}
		if (i == count) {
			p[i] = m0;
			count++;
		}

	}

	count = 0;
	while (count < n) {
		generator(r, p_P, p_Q);
		mpz_mod_ui(r, r, N);
		int n0 = mpz_get_ui(r);
		int i;
		for (i = 0; i < count; ++i) {
			if (q[i] == n0) {
				break;
			}
		}
		if (i == count) {
			q[i] = n0;
			count++;
		}

	}

	mpz_clears(r, p_P.x, p_P.y, p_P.z, p_Q.x, p_Q.y, p_Q.z, NULL);

}

void ECCDRBG::randomINC(int *q, int *p, int M, int m, int N, int n) {
	eccdrbg_point p_P;
	eccdrbg_point p_Q;
	mpz_t r, tmp;
	int count;
	size_t len_seed;
	unsigned char *buf_seed;
	unsigned char out[ECC_DRBG_MAX_LEN];

	mpz_inits(r, p_P.x, p_P.y, p_P.z, p_Q.x, p_Q.y, p_Q.z, NULL);

	buf_seed = (unsigned char *) mpz_limbs_read(seed);
	len_seed = mpz_size(seed) * sizeof(mp_limb_t) / sizeof(unsigned char);

	sha256_starts(&hashP, 0);
	sha256_update(&hashP, buf_seed, len_seed);
	sha256_update(&hashP, (const unsigned char *) &M, sizeof(unsigned int)/sizeof(char));
	sha256_update(&hashP, (const unsigned char *) &N, sizeof(unsigned int)/sizeof(char));
	sha256_update(&hashP, (const unsigned char *) &m, sizeof(unsigned int)/sizeof(char));
	sha256_update(&hashP, (const unsigned char *) &n, sizeof(unsigned int)/sizeof(char));
	sha256_clone(&hashQ, &hashP);

	//P
	sha256_update(&hashP, (unsigned char *) mpz_limbs_read(P.x),
			(size_t) (mpz_size(P.x) * sizeof(mp_limb_t) / sizeof(unsigned char)));
	sha256_finish(&hashP, out);

	mpz_roinit_n(tmp, (mp_limb_t *) out,
			(mp_size_t) (ECC_DRBG_MAX_LEN * sizeof(unsigned char)
					/ sizeof(mp_limb_t)));
	mpz_set(r, tmp);
	ecc_mul(p_P, r, P);

	//Q
	sha256_update(&hashQ, (unsigned char *) mpz_limbs_read(Q.x),
			(size_t) (mpz_size(Q.x) * sizeof(mp_limb_t) / sizeof(unsigned char)));
	sha256_finish(&hashQ, out);

	mpz_roinit_n(tmp, (mp_limb_t *) out,
			(mp_size_t) (ECC_DRBG_MAX_LEN * sizeof(unsigned char)
					/ sizeof(mp_limb_t)));
	mpz_set(r, tmp);
	ecc_mul(p_Q, r, Q);

	count = 0;
	while (count < m) {
		generator(r, p_P, p_Q);
		mpz_mod_ui(r, r, M);
		int m0 = mpz_get_ui(r);
		int i;
		for (i = 0; i < count; ++i) {
			if (p[i] == m0) {
				m0++;
				m0 = m0 % M;
				i = 0;
			}
		}
		p[i] = m0;
		count++;
	}

	count = 0;
	while (count < n) {
		generator(r, p_P, p_Q);
		mpz_mod_ui(r, r, N);
		int n0 = mpz_get_ui(r);
		int i;
		for (i = 0; i < count; ++i) {
			if (q[i] == n0) {
				n0++;
				n0 = n0 % N;
				i = 0;
			}
		}
		q[i] = n0;
		count++;
	}

	mpz_clears(r, p_P.x, p_P.y, p_P.z, p_Q.x, p_Q.y, p_Q.z, NULL);
}
