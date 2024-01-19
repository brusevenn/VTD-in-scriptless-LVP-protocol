#ifndef __LHP_PARAM__
#define __LHP_PARAM__

#include <gmp.h>

class LHP_param_t {
public:
    mpz_t T;
    mpz_t N;
    mpz_t g;
    mpz_t h;

    LHP_param_t();
    ~LHP_param_t();
};

#endif
