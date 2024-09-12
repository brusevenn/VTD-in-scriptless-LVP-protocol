#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include "lhp.h"
#include "util.h"

void LHP_PSetup ( LHP_param_t& params , uint64_t lambda , uint64_t T )//生成pp
{
	mpz_t p , q , temp , temp2 ;
	mpz_init2 ( p , lambda ) ;
	mpz_init2 ( q , lambda ) ;
	mpz_init ( temp ) ;
	mpz_init_set_ui ( temp2 , 2 ) ;
	mpz_set_ui ( params . T , T ) ;
	generate_strong_prime ( p , lambda / 2 ) ; // Generate p = 2p'+ 1
	generate_strong_prime ( q , lambda / 2 ) ; // Generate q = 2q'+ 1
	/*gmp_printf ( "Generated %d-bit Strong Primes\n p: %Zx\n q: %Zx\n" , lambda , p , q ) ;*/
	mpz_mul ( params.N , p , q ) ;//N是lambda位的
	/*
	 * 1. Generate g = -g^2 mod N
	 * 2. Generate t1 = phi(n)
	 * 3. Compute t2 = 2^T mod (t1/2)
	 * 4. Compute h = g^t2 mod N
	 * */
	random_group_element ( params.g , params.N ) ;
	mpz_powm_ui ( params.g , params.g , 2 , params.N ) ;
	mpz_invert ( params.g , params.g , params.N ) ;
	totient_n ( temp , p , q ) ; // temp has phi(n)
	mpz_div_ui ( temp , temp , 2 ) ;
	mpz_powm ( params.h , temp2 , params.T , temp ) ;
	// Confirm with aravind \|/
	mpz_powm ( params.h , params.g , params.h , params.N ) ;//h=g^t2 mod N
	mpz_clear ( p ) ;
	mpz_clear ( q ) ;
	mpz_clear ( temp ) ;
	mpz_clear ( temp2 ) ;
	
}

// The function won't hash anything
// Give the function a hashed string
void LHP_PGen (LHP_Puzzle& puzzle , LHP_param_t& pp , mpz_t s )//成puzzle
{
	gmp_randstate_t state;
	gmp_randinit_default( state ) ;
	gmp_randseed_ui ( state , rand() ) ;
	mpz_t r , N2 , temp ;
	mpz_init ( r ) ;
	mpz_init ( N2 ) ;
	mpz_init ( temp ) ;
	mpz_pow_ui ( N2 , pp.N , 2 ) ;//N2=N^2
	mpz_urandomm ( r , state , N2 ) ;//r随机生成
	mpz_powm ( puzzle.u , pp.g , r , pp.N ) ;//u=g^r mod N
	mpz_mul ( temp , r , pp.N ) ;//temp=r*N
	mpz_powm ( puzzle.v , pp.h , temp , N2 ) ;//v=h^temp mod N2
	mpz_add_ui ( temp , pp.N , 1 ) ;//temp=N+1
	mpz_powm ( temp , temp , s , N2 ) ;//temp=temp^s mod N2，这里的s会换成x_i，x_i是拉格朗日插值生成的share
	mpz_mul ( puzzle.v , puzzle.v , temp ) ;
	mpz_mod ( puzzle.v , puzzle.v , N2 ) ;//v=v*temp mod N2
	mpz_clear ( r ) ;
	mpz_clear ( N2 ) ;
	mpz_clear ( temp ) ;
	gmp_randclear ( state );
}

