#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>
#include "nizk.h"
#include "util.h"
#include "Shamir.h"
#include "lhp.h"
#include "vtd.h"
#include <vector>

//本节为了适配拉格朗日插值，数组统一从i =1 开始
//Setup_VTD函数，输入安全参数λ，输出公共参考字符串crs，crs = (crs_rang,pp)
void Setup_VTD(VTD_crs_t& crs , uint64_t lambda, uint64_t T)  {
	mpz_t crs_range;
	mpz_init(crs_range);
	ZKSetup(lambda, crs_range);//生成NIZK的公共参考字符串crs_range
	LHP_PSetup(crs.params, lambda, T);//生成LHP的公共参考字符串pp
	mpz_set(crs.crs_range, crs_range);
	mpz_clear(crs_range);
}



//Commit_VTD函数，w是witness，也就是sk；crs是公共参考字符串，g是生成元，n是份额总数，t是门限
//输出是puzzle数组Z[i]、proofs[i]是NIZK的证明、H是公钥，w_shares[i]是份额、H_shares[i]是公钥的份额
void Commit_VTD(LHP_Puzzle* Z, Proof* proofs, mpz_t H, mpz_t* w_shares, mpz_t* H_shares, mpz_t w, VTD_crs_t& crs, mpz_t g, mpz_t prime, int n, int t , uint64_t lambda) {
	generate_shares(n, t, w, prime, w_shares);//生成份额，n为份额数，t为门限值，w为秘密，prime为大素数，份额保存在shares数组中
	for (int i = 1; i <= n; i++) {
		mpz_powm(H_shares[i], g, w_shares[i], crs.crs_range);
	}
	LHP_Puzzle puzzle;
	for (int i = 1; i <= n; i++) {
		LHP_PGen(puzzle, crs.params, w_shares[i]);//生成puzzle
		Z[i] = puzzle;
	}
	mpz_t hash;
	mpz_init(hash);
	//生成NIZK的证明proofs[i]
	Proof proof;
	for (int i = 1; i <= n; i++) {
		hash_mpz(Z[i].u, Z[i].v, hash);
		ZKprove(proof, crs.crs_range, hash , w_shares[i] ,lambda);//生成NIZK的证明
		proofs[i] = proof;
	}
	mpz_clear(hash);
	
}

	//crs是来的公共参考字符串，H是公钥pk，Z[i]是puzzle，H_shares是pk的份额，proofs是pi[i],n是份额数, t是门限阈值
void Verify_VTD(VTD_crs_t& crs, LHP_Puzzle* Z, mpz_t H, mpz_t* H_shares, mpz_t* w_shares, Proof* proofs, mpz_t g, mpz_t prime,  int n, int t) {
	/*
	1、验证h[i]*h[j] ?= pk;
	2、ZKverify(mpz_t crs_range, mpz_t z, Proof* pi);
	3、验证h_i ?= g_i
	*/
	//计算拉格朗日基数
	
	std::vector<double> l(t+1, 0);
	std::vector<int> x(t + 1, 0);
	for (int i = 0; i <= t; i++) {
		x[i] = i;
	}

	lagrange_coefficients(t, x.data(), l.data());
	//前n-1个h[i]的乘积
	mpz_t sum, pow_result, l_mpz, hash, w_shares_res;
	mpz_inits(sum, pow_result, l_mpz, hash, w_shares_res, NULL);
	mpz_set_ui(sum, 1);

	for (int i = 1; i <= t - 1; i++) {
		double l_inv = 1 / l[i];
		mpz_set_d(l_mpz, l_inv);
		mpz_powm(pow_result, H_shares[i], l_mpz ,prime);
		mpz_mul(sum, sum, pow_result);
	}
	
	int cmp;
	mpz_mul(sum, sum, H_shares[t]);

	double l_inv = 1 / l[t];
	mpz_set_d(l_mpz, l_inv);
	mpz_powm(H, H, l_mpz, prime);
	cmp = mpz_cmp(sum, H);

	int ZK_res = 1;

	for (int i = 1; i <= n; i++) {
		hash_mpz(Z[i].u, Z[i].v, hash);
		ZK_res *= ZKverify(crs.crs_range, hash, proofs[i]);\
		mpz_set_ui(hash, 0); //重置hash
	}

	int Share_res = 1;

	for (int i = 1; i <= n; i++) {
		mpz_powm(w_shares_res, g, w_shares[i], crs.crs_range);
		if (!mpz_cmp(H_shares[i], w_shares_res)) {
			exit;
		}
		mpz_set_ui(w_shares_res, 0); //重置w_shares_res
	}
	mpz_clears(sum, pow_result, l_mpz, hash, w_shares_res, NULL);
}