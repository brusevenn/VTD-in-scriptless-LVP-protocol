#include "Shamir.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <gmp.h>
#include <util.h>
#include <vector>


// 生成多项式，多项式的系数保存在coefficients数组中，第一个多项式的系数为secret，后面的多项式的系数为随机数，degree为门限值-1
void generate_polynomial(int degree, mpz_t secret, mpz_t prime, mpz_t* coefficients ) {
    mpz_set(coefficients[0], secret);
    for (int i = 1; i <= degree; i++) {
        generate_random(coefficients[i], 100);
    }
}

// 计算多项式的值，x为自变量，prime为大素数，每一项的x的指数为i，系数为coefficients[i], 
void evaluate_polynomial(int degree, mpz_t* coefficients, int x, mpz_t prime , mpz_t result) {
    mpz_set_ui(result, 0);//result = 0
    for (int i = degree; i >= 0; i--) {
        mpz_mul_ui(result, result, x);//result = result * x
        mpz_add(result, result, coefficients[i]);//result = result + coefficients[i]
    }
    mpz_mod(result, result, prime);//result = result%prime
 }

// 生成份额，n为份额数，t为门限值，secret为秘密，prime为大素数，份额保存在shares数组中
void generate_shares(int n, int t, mpz_t secret, mpz_t prime, mpz_t* shares) {
    std::vector<mpz_t> coefficients(t);  // 分配内存
    for (int i = 0; i < t; i++) {
        mpz_init(coefficients[i]);  // 初始化每个元素
    }

    std::vector<double> l(t + 1, 0); //拉格朗日插值 
    mpz_t result;
    mpz_init(result);
    //[1,t-1]按照原始的拉格朗日份额产生
    generate_polynomial(t - 1, secret, prime, coefficients.data());// 生成多项式，degree为t - 1，secret为秘密，prime为大素数，系数保存在coefficients数组中
    for (int i = 1; i <= t - 1; i++) {
        evaluate_polynomial(t - 1, coefficients.data(), i, prime, result);//计算多项式的值，x为i，从1计算到t-1，计算多项式的值，prime为大素数，结果保存在result中
        mpz_mod(result, result, prime);//result = result%prime)

        mpz_set(shares[i], result);//shares[i] = result
    }
    //[t,n]按照VTD的方案计算
    std::vector<int> x(n + 1);
    for (int i = 0; i <= n; i++) {
        x[i] = i;
    }
    lagrange_coefficients(t, x.data(), l.data());
    mpf_t ma, mb, mc, sum;
    mpf_init(ma);
    mpf_init(mb);
    mpf_init(mc);
    mpf_init(sum);
    for (int i = 1; i < t; i++) {
        mpf_set_z(ma, shares[i]);//ma = shares[i])
        mpf_set_d(mb, l[i]);//mb = l[i]
        mpf_mul(ma, ma, mb);//ma = ma * mb
        mpf_add(sum, ma, sum);//sum = ma + sum
    }
    mpf_set_z(mc, secret);//mc = secret
    mpf_t msub, mdiv;
    mpf_init(msub);
    mpf_init(mdiv);
    mpf_sub(msub, mc, sum);//msub = mc - sum
    mpf_set_d(mb, l[t]);//mb = l[i]
    for (int i = t; i <= n; i++) {
        mpf_div(mdiv, msub, mb);
        mpz_set_f(shares[i], mdiv);//shares[i] = mdiv
        mpz_mod(shares[i], shares[i], prime);
    }
    mpz_clear(result);
    mpf_clear(ma);
    mpf_clear(mb);
    mpf_clear(mc);
    mpf_clear(sum);
    mpf_clear(msub);
    mpf_clear(mdiv);
    for (int i = 0; i < t; i++) {
        mpz_clear(coefficients[i]);  // 清除每个元素
    }
}

// 计算拉格朗日插值多项式的系数，计算的是l_i(0)的情况，计算l_1(0), l_2(0), ..., l_t(0)的值，x为自变量，coefficients保存系数
void lagrange_coefficients(int t, int* x, double* coefficients) {
    for (int i = 1; i <= t; i++) {
        
        coefficients[i] = 1.0;
        for (int j = 1; j <= t; j++) {
            if (i != j) {
                coefficients[i] *= (double)(x[j]) / (x[j] - x[i]);
            }
        }
    }
}

//恢复秘密，t为门限值，x为自变量，y为因变量，prime为大素数
void recover_secret(int t, int* x, mpz_t* y, mpz_t prime ,mpz_t recover_secret) {
    std::vector<double> l(t + 1, 0);
    lagrange_coefficients(t, x, l.data());//计算拉格朗日插值多项式的系数，计算的是l_i(0)的情况，计算l_1(0), l_2(0), ..., l_t(0)的值
    mpf_t ma , mb , mc ,sum;
    mpf_init(ma);
    mpf_init(mb);
    mpf_init(mc);
    mpf_init(sum);
    for (int i = 1; i <= t; i++) {
        mpf_set_z(ma, y[i]);//ma = y[i]
        mpf_set_d(mb, l[i]);//mb = l[i]
        mpf_mul(mc, ma, mb);//mc = ma * mb
        mpf_add(sum, sum, mc);//sum = sum + mc
    }
    mpz_set_f(recover_secret, sum);//recover_secret = sum
    mpz_mod(recover_secret, recover_secret, prime);//recover_secret = recover_secret%prime
    mpf_clear(ma);
    mpf_clear(mb);
    mpf_clear(mc);
    mpf_clear(sum);
}