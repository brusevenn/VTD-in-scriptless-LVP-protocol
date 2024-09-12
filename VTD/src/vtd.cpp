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

//����Ϊ�������������ղ�ֵ������ͳһ��i =1 ��ʼ
//Setup_VTD���������밲ȫ�����ˣ���������ο��ַ���crs��crs = (crs_rang,pp)
void Setup_VTD(VTD_crs_t& crs , uint64_t lambda, uint64_t T)  {
	mpz_t crs_range;
	mpz_init(crs_range);
	ZKSetup(lambda, crs_range);//����NIZK�Ĺ����ο��ַ���crs_range
	LHP_PSetup(crs.params, lambda, T);//����LHP�Ĺ����ο��ַ���pp
	mpz_set(crs.crs_range, crs_range);
	mpz_clear(crs_range);
}



//Commit_VTD������w��witness��Ҳ����sk��crs�ǹ����ο��ַ�����g������Ԫ��n�Ƿݶ�������t������
//�����puzzle����Z[i]��proofs[i]��NIZK��֤����H�ǹ�Կ��w_shares[i]�ǷݶH_shares[i]�ǹ�Կ�ķݶ�
void Commit_VTD(LHP_Puzzle* Z, Proof* proofs, mpz_t H, mpz_t* w_shares, mpz_t* H_shares, mpz_t w, VTD_crs_t& crs, mpz_t g, mpz_t prime, int n, int t , uint64_t lambda) {
	generate_shares(n, t, w, prime, w_shares);//���ɷݶnΪ�ݶ�����tΪ����ֵ��wΪ���ܣ�primeΪ���������ݶ����shares������
	for (int i = 1; i <= n; i++) {
		mpz_powm(H_shares[i], g, w_shares[i], crs.crs_range);
	}
	LHP_Puzzle puzzle;
	for (int i = 1; i <= n; i++) {
		LHP_PGen(puzzle, crs.params, w_shares[i]);//����puzzle
		Z[i] = puzzle;
	}
	mpz_t hash;
	mpz_init(hash);
	//����NIZK��֤��proofs[i]
	Proof proof;
	for (int i = 1; i <= n; i++) {
		hash_mpz(Z[i].u, Z[i].v, hash);
		ZKprove(proof, crs.crs_range, hash , w_shares[i] ,lambda);//����NIZK��֤��
		proofs[i] = proof;
	}
	mpz_clear(hash);
	
}

	//crs�����Ĺ����ο��ַ�����H�ǹ�Կpk��Z[i]��puzzle��H_shares��pk�ķݶproofs��pi[i],n�Ƿݶ���, t��������ֵ
void Verify_VTD(VTD_crs_t& crs, LHP_Puzzle* Z, mpz_t H, mpz_t* H_shares, mpz_t* w_shares, Proof* proofs, mpz_t g, mpz_t prime,  int n, int t) {
	/*
	1����֤h[i]*h[j] ?= pk;
	2��ZKverify(mpz_t crs_range, mpz_t z, Proof* pi);
	3����֤h_i ?= g_i
	*/
	//�����������ջ���
	
	std::vector<double> l(t+1, 0);
	std::vector<int> x(t + 1, 0);
	for (int i = 0; i <= t; i++) {
		x[i] = i;
	}

	lagrange_coefficients(t, x.data(), l.data());
	//ǰn-1��h[i]�ĳ˻�
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
		mpz_set_ui(hash, 0); //����hash
	}

	int Share_res = 1;

	for (int i = 1; i <= n; i++) {
		mpz_powm(w_shares_res, g, w_shares[i], crs.crs_range);
		if (!mpz_cmp(H_shares[i], w_shares_res)) {
			exit;
		}
		mpz_set_ui(w_shares_res, 0); //����w_shares_res
	}
	mpz_clears(sum, pow_result, l_mpz, hash, w_shares_res, NULL);
}