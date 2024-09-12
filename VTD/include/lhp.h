#include <stdint.h>

#include "params.h"
#include "puzzle.h"

#ifndef __LHP_HEADER__
#define __LHP_HEADER__

void LHP_PSetup ( LHP_param_t& params , uint64_t lambda , uint64_t ) ;
void LHP_PGen(LHP_Puzzle& puzzle, LHP_param_t& pp, mpz_t s);

#endif
