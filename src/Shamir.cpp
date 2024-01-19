#include "Shamir.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <gmp.h>
#include <util.h>
#include <vector>


// ���ɶ���ʽ������ʽ��ϵ��������coefficients�����У���һ������ʽ��ϵ��Ϊsecret������Ķ���ʽ��ϵ��Ϊ�������degreeΪ����ֵ-1
void generate_polynomial(int degree, mpz_t secret, mpz_t prime, mpz_t* coefficients ) {
    mpz_set(coefficients[0], secret);
    for (int i = 1; i <= degree; i++) {
        generate_random(coefficients[i], 100);
    }
}

// �������ʽ��ֵ��xΪ�Ա�����primeΪ��������ÿһ���x��ָ��Ϊi��ϵ��Ϊcoefficients[i], 
void evaluate_polynomial(int degree, mpz_t* coefficients, int x, mpz_t prime , mpz_t result) {
    mpz_set_ui(result, 0);//result = 0
    for (int i = degree; i >= 0; i--) {
        mpz_mul_ui(result, result, x);//result = result * x
        mpz_add(result, result, coefficients[i]);//result = result + coefficients[i]
    }
    mpz_mod(result, result, prime);//result = result%prime
 }

// ���ɷݶnΪ�ݶ�����tΪ����ֵ��secretΪ���ܣ�primeΪ���������ݶ����shares������
void generate_shares(int n, int t, mpz_t secret, mpz_t prime, mpz_t* shares) {
    std::vector<mpz_t> coefficients(t);  // �����ڴ�
    for (int i = 0; i < t; i++) {
        mpz_init(coefficients[i]);  // ��ʼ��ÿ��Ԫ��
    }

    std::vector<double> l(t + 1, 0); //�������ղ�ֵ 
    mpz_t result;
    mpz_init(result);
    //[1,t-1]����ԭʼ���������շݶ����
    generate_polynomial(t - 1, secret, prime, coefficients.data());// ���ɶ���ʽ��degreeΪt - 1��secretΪ���ܣ�primeΪ��������ϵ��������coefficients������
    for (int i = 1; i <= t - 1; i++) {
        evaluate_polynomial(t - 1, coefficients.data(), i, prime, result);//�������ʽ��ֵ��xΪi����1���㵽t-1���������ʽ��ֵ��primeΪ�����������������result��
        mpz_mod(result, result, prime);//result = result%prime)

        mpz_set(shares[i], result);//shares[i] = result
    }
    //[t,n]����VTD�ķ�������
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
        mpz_clear(coefficients[i]);  // ���ÿ��Ԫ��
    }
}

// �����������ղ�ֵ����ʽ��ϵ�����������l_i(0)�����������l_1(0), l_2(0), ..., l_t(0)��ֵ��xΪ�Ա�����coefficients����ϵ��
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

//�ָ����ܣ�tΪ����ֵ��xΪ�Ա�����yΪ�������primeΪ������
void recover_secret(int t, int* x, mpz_t* y, mpz_t prime ,mpz_t recover_secret) {
    std::vector<double> l(t + 1, 0);
    lagrange_coefficients(t, x, l.data());//�����������ղ�ֵ����ʽ��ϵ�����������l_i(0)�����������l_1(0), l_2(0), ..., l_t(0)��ֵ
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