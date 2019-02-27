/*
 * lattdrbg.cpp
 *
 *  Created on: 2018年8月12日
 *      Author: lzj
 */

#include "lattdrbg.h"


void LATTDRBG::hexStrToByte(const char* source, unsigned char* dest,
		int sourceLen) {
	short i;
	unsigned char highByte, lowByte;
	if (sourceLen > 64) {
		sourceLen = 64;
	}

	for (i = 0; i < sourceLen; i += 2) {
		highByte = toupper(source[i]);
		lowByte = toupper(source[i + 1]);

		if (highByte > 0x39)
			highByte -= 0x37;
		else
			highByte -= 0x30;

		if (lowByte > 0x39)
			lowByte -= 0x37;
		else
			lowByte -= 0x30;

		dest[i / 2] = (highByte << 4) | lowByte;
	}
}

LATTDRBG::LATTDRBG() {
	__m1 = 0;
	__m2 = 0;
	__m = 0;
	__q = 0;
	__n = 0;
	_A = NULL;
	__k = 0;
	sha256_init(&__hash);
	std::memset(__seed, 0, sizeof(__seed));
}

LATTDRBG::~LATTDRBG() {
	if (_A != 0) {
		for (int i = 0; i < __n; ++i) {
			delete[] _A[i];
		}
		delete[] _A;
	}
	sha256_free(&__hash);
	std::memset(__seed, 0, sizeof(__seed));
}

void LATTDRBG::initMatrixA() {
	__m = __m1 + __m2;
	_A = new int*[__n];
	for (int i = 0; i < __n; ++i) {
		_A[i] = new int[__m];
	}

	for (int i = 0; i < __n; ++i) {
		for (int j = 0; j < __m; ++j) {
			_A[i][j] = j^i %__q;
		}
	}

}

void LATTDRBG::set_m1(int m1) {
	__m1 = m1;

}

void LATTDRBG::set_seedHexStr(const char * seed) {
	hexStrToByte(seed, __seed, strlen(seed));

}

void LATTDRBG::set_seed(unsigned char* seed[32]) {
	std::memcpy(__seed, seed, 32);
}

void LATTDRBG::set_m2(int m2) {
	__m2 = m2;
}

void LATTDRBG::set_n(int n) {
	__n = n;
}

void LATTDRBG::set_q(int q) {
	__q = q;
}

void LATTDRBG::set_k(int k) {
	__k = k;
}

void LATTDRBG::random(int *p, int *q, int M, int m, int N, int n) {

	if (_A == NULL) {
		return;
	}
	mpz_t mpz_c, mpz_M, mpz_m, mpz_N, mpz_n, mpz_y, mpz_tmp;
	int len = __k * 32;
	int count;
	char *str, *str_y;
	unsigned char *c = new unsigned char[len];
	unsigned int *y = new unsigned int[__n];
	str_y = new char[4097];
	std::memset(str_y, 0, 4097);
	std::memset(y, 0, sizeof(int) * __n);
	std::memcpy(c, __seed, 32);

	for (int i = 0; i < __k - 2; ++i) {
		unsigned char out[32];
		sha256_starts(&__hash, 0);
		switch (i % 4) {
		case 0:
			sha256_update(&__hash, (unsigned char *) &M, sizeof(int));
			break;
		case 1:
			sha256_update(&__hash, (unsigned char *) &m, sizeof(int));
			break;
		case 2:
			sha256_update(&__hash, (unsigned char *) &N, sizeof(int));
			break;
		case 3:
			sha256_update(&__hash, (unsigned char *) &n, sizeof(int));
			break;

		}
		sha256_update(&__hash, c + i * 32, 32);
		sha256_finish(&__hash, out);
		std::memcpy(c + (i * 32 + 32), out, 32);
	}

	mpz_roinit_n(mpz_c, (mp_limb_t *) c,
			sizeof(char) * len / sizeof(mp_limb_t));

	mpz_init_set_ui(mpz_M, M);
	mpz_init_set_ui(mpz_m, m);
	mpz_init_set_ui(mpz_N, N);
	mpz_init_set_ui(mpz_n, n);
	mpz_init(mpz_tmp);

	str = new char[__m + 2];
	for (int i = 0; i <= 9; ++i) {
		if ((M >> i) & 0x1) {
			str[9 - i] = '1';
		} else {
			str[9 - i] = '0';
		}
	}

	for (int i = 0; i <= 6; ++i) {
		if ((m >> i) & 0x1) {
			str[16 - i] = '1';
		} else {
			str[16 - i] = '0';
		}
	}

	for (int i = 0; i <= 23; ++i) {
		if ((N >> i) & 0x1) {
			str[39 - i] = '1';
		} else {
			str[39 - i] = '0';
		}
	}

	for (int i = 0; i <= 6; ++i) {
		if ((n >> i) & 0x1) {
			str[45 - i] = '1';
		} else {
			str[45 - i] = '0';
		}
	}
	mpz_get_str(str + 46, 2, mpz_c);

	for (int i = 0; i < __n; ++i) {
		unsigned long tmp = 0;
		for (int j = 0; j < __m; ++j) {
			if (str[j] == '1') {
				tmp += _A[i][j];
				tmp = tmp % __q;
			}
		}
		y[i] = tmp;
	}

	mpz_roinit_n(mpz_y, (mp_limb_t *) y,
			sizeof(unsigned int) * __n / sizeof(mp_limb_t));
	mpz_get_str(str_y, 2, mpz_y);

	count = 0;
	while (count < m) {
		char str_tmp[11] = { 0 };
		std::memcpy(str_tmp, str_y + count * 10, 10);
		mpz_set_str(mpz_tmp, str_tmp, 2);
		mpz_mod_ui(mpz_tmp, mpz_tmp, M);
		int m0 = mpz_get_ui(mpz_tmp);
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
		char str_tmp[25] = { 0 };
		std::memcpy(str_tmp, str_y + count * 24 + 10 * m, 24);
		mpz_set_str(mpz_tmp, str_tmp, 2);
		mpz_mod_ui(mpz_tmp, mpz_tmp, N);
		int n0 = mpz_get_ui(mpz_tmp);
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

	delete[] c;
	delete[] y;
	delete[] str;
	delete[] str_y;
	mpz_clears(mpz_M, mpz_m, mpz_N, mpz_n, mpz_tmp, NULL);

}
