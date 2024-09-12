#ifndef __LHP_PUZZLE__
#define __LHP_PUZZLE__

#include <gmp.h>

class LHP_Puzzle{
public:
    mpz_t u;
    mpz_t v;

    LHP_Puzzle();
    ~LHP_Puzzle();
};

#endif
