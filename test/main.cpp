#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>
#include "lhp.h"
#include "util.h"
#include "vtd.h"
#include "nizk.h"
#include "Shamir.h"
#include <util.h>
#include <chrono>
#include <iostream>

const int n = 256;//份额
const int t = 128;//门限
const uint64_t lambda = 100;//安全参数
const uint64_t T = 16;

int main() {
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Setup~~~~~~~~~~~~~~~~~~~~~~~~~~
	/*
	Setup:预先生成VTD需要的参数：crs、w、g
	*/
	auto start = std::chrono::high_resolution_clock::now();

	VTD_crs_t crs;
	Setup_VTD(crs, lambda, T);
	gmp_printf("crs_range = %Zd\n", crs.crs_range);

	mpz_t w;//秘密
	mpz_init(w);
	//生成w
	gmp_randstate_t state;
	gmp_randinit_default(state);
	gmp_randseed_ui(state, rand());  // 使用当前时间作为随机种子;
	mpz_urandomb(w, state, lambda);  // 生成一个随机的密钥
	gmp_randclear(state);
	gmp_printf("w = %Zd\n", w);

	mpz_t g;//生成元
	mpz_init(g);
	random_group_element(g, w);  //生成g
	gmp_printf("g = %Zd\n", g);

	mpz_t H;
	mpz_init(H);
	mpz_powm(H, g, w, crs.crs_range);//H = g^w mod crs_range公钥
	gmp_printf("H = %Zd\n", H);

	mpz_t prime;
	mpz_init(prime);
	generate_prime(prime, lambda);//Shamir门限所需的大素数prime
	gmp_printf("prime = %Zd\n", prime);

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	std::cout << "Setup time: " << diff.count() << " seconds" << std::endl;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Commit~~~~~~~~~~~~~~~~~~~~~~~~~~
	 start = std::chrono::high_resolution_clock::now();

	LHP_Puzzle Z[n + 1];
	Proof proofs [n + 1];
	mpz_t w_shares[n + 1];
	mpz_t H_shares [n + 1];
	//生成Z、proofs、H、w_shares、H_shares
	for (int i = 0; i <= n; i++) {
		mpz_init(w_shares[i]);
		mpz_init(H_shares[i]);
	}

	Commit_VTD(Z, proofs, H, w_shares, H_shares, w, crs, g, prime, n, t, lambda);

	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	std::cout << "Commit time: " << diff.count() << " seconds" << std::endl;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Verify~~~~~~~~~~~~~~~~~~~~~~~~~~
	start = std::chrono::high_resolution_clock::now();

	Verify_VTD(crs, Z,H, H_shares, w_shares, proofs, prime, g, n, t);

	end = std::chrono::high_resolution_clock::now();
	diff = end - start;
	std::cout << "Verify time: " << diff.count() << " seconds" << std::endl;

	mpz_clear(w);
	mpz_clear(g);
	mpz_clear(prime);
	mpz_clear(H);
	for (int i = 0; i <= n; i++) {
		mpz_clear(w_shares[i]);
		mpz_clear(H_shares[i]);
	}
	system("pause");//暂停
	return 0;
}
