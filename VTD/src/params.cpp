#include "gmp.h"
#include "params.h"

LHP_param_t::LHP_param_t() {
    mpz_init(T);
    mpz_init(N);
    mpz_init(g);
    mpz_init(h);
}

LHP_param_t::~LHP_param_t() {
    mpz_clear(T);
    mpz_clear(N);
    mpz_clear(g);
    mpz_clear(h);

}

