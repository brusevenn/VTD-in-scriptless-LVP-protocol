#include "gmp.h"

#include "puzzle.h"

LHP_Puzzle::LHP_Puzzle() {
    mpz_init(u);
    mpz_init(v);
}

LHP_Puzzle::~LHP_Puzzle() {
    mpz_clear(u);
    mpz_clear(v);
}

