#pragma once
#include <gmp.h>
#ifndef __Shamir__
#define __Shamir__


//���ɶ���ʽ������ʽ��ϵ��������coefficients�����У���һ������ʽ��ϵ��Ϊsecret������Ķ���ʽ��ϵ��Ϊ�����
void generate_polynomial(int degree, mpz_t secret, mpz_t prime, mpz_t* coefficients );
//�������ʽ��ֵ��xΪ�Ա�����primeΪ��������ÿһ���x��ָ��Ϊi��ϵ��Ϊcoefficients[i],
void evaluate_polynomial(int degree, mpz_t* coefficients, int x, mpz_t prime, mpz_t result);
//���ɷݶnΪ�ݶ�����tΪ����ֵ��secretΪ���ܣ�primeΪ���������ݶ����shares������
void generate_shares(int n, int t, mpz_t secret, mpz_t prime, mpz_t* shares);
//�����������ղ�ֵ����ʽ��ϵ�����������l_i(0)�����������l_1(0), l_2(0), ..., l_t(0)��ֵ��xΪ�Ա�����coefficients����ϵ��
void lagrange_coefficients(int t, int* x, double* coefficients);
//�ָ����ܣ�tΪ����ֵ��xΪ�Ա�����yΪ�������primeΪ������
void recover_secret(int t, int* x, mpz_t* y, mpz_t prime, mpz_t recover_secret);
#endif
