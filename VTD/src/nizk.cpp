#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>
#include "lhp.h"
#include "util.h"
#include "nizk.h"
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <string>

// ZKSetup函数，输入安全参数λ，输出公共参考字符串crs
void ZKSetup(uint64_t lambda, mpz_t& crs_range) {
    generate_random(crs_range, lambda);
}

// ZKprove函数，输入公共参考字符串crs、语句z和证明w，输出证明π
void ZKprove(Proof& proof, mpz_t crs_range, mpz_t z, mpz_t w, uint64_t lambda) {
    mpz_t r;
    mpz_init(r);
    generate_random(r, lambda);
    mpz_mul(proof.P, w, crs_range);
    mpz_mul(proof.R, r, crs_range);
    mpz_mul(proof.S, z, w);
    mpz_add(proof.S, proof.S, r);
    mpz_clear(r);
}

// ZKverify函数，输入公共参考字符串crs、语句x和证明π，输出验证结果
int ZKverify(mpz_t crs_range, mpz_t z, Proof& proof) {
    mpz_t left, right;
    mpz_init(left);
    mpz_init(right);
    mpz_mul(left, proof.S, crs_range);
    mpz_mul(right, z, proof.P);
    mpz_add(right, right, proof.R);
    int result = mpz_cmp(left, right);
    mpz_clear(left);
    mpz_clear(right);
    return result;
}

//输入两个mpz_t类型的数，使用SHA256哈希函数计算哈希值，输出到hash中

void hash_mpz(mpz_t u, mpz_t v, mpz_t &hash) {
    std::string u_str = mpz_get_str(NULL, 10, u);
    std::string v_str = mpz_get_str(NULL, 10, v);
    std::string concat_str = u_str + v_str;
    unsigned char hash_result[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)concat_str.c_str(), concat_str.length(), hash_result);
    mpz_import(hash, SHA256_DIGEST_LENGTH, 1, sizeof(hash_result[0]), 0, 0, hash_result);
}