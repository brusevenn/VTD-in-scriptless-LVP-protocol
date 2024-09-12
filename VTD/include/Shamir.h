#pragma once
#include <gmp.h>
#ifndef __Shamir__
#define __Shamir__


//生成多项式，多项式的系数保存在coefficients数组中，第一个多项式的系数为secret，后面的多项式的系数为随机数
void generate_polynomial(int degree, mpz_t secret, mpz_t prime, mpz_t* coefficients );
//计算多项式的值，x为自变量，prime为大素数，每一项的x的指数为i，系数为coefficients[i],
void evaluate_polynomial(int degree, mpz_t* coefficients, int x, mpz_t prime, mpz_t result);
//生成份额，n为份额数，t为门限值，secret为秘密，prime为大素数，份额保存在shares数组中
void generate_shares(int n, int t, mpz_t secret, mpz_t prime, mpz_t* shares);
//计算拉格朗日插值多项式的系数，计算的是l_i(0)的情况，计算l_1(0), l_2(0), ..., l_t(0)的值，x为自变量，coefficients保存系数
void lagrange_coefficients(int t, int* x, double* coefficients);
//恢复秘密，t为门限值，x为自变量，y为因变量，prime为大素数
void recover_secret(int t, int* x, mpz_t* y, mpz_t prime, mpz_t recover_secret);
#endif
