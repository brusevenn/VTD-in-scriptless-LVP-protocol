#ifndef __LHP_NIZK__
#define __LHP_NIZK__	
#include <gmp.h>
#include <stdint.h>
//生成结构体Proof
class Proof {
public:
    mpz_t P;
    mpz_t R;
    mpz_t S;

    Proof() {
        mpz_init(P);
        mpz_init(R);
        mpz_init(S);
    }
    ~Proof() {
        mpz_clear(P);
        mpz_clear(R);
        mpz_clear(S);
    }
};

void ZKSetup(uint64_t lambda, mpz_t &crs_range);
void ZKprove(Proof &proof, mpz_t crs_range, mpz_t z, mpz_t w, uint64_t lambda);
int ZKverify(mpz_t crs_range, mpz_t z, Proof& proof);

void hash_mpz(mpz_t u, mpz_t v, mpz_t &hash);
#endif