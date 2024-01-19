#pragma once

#ifndef __VTD__
#define __VTD__
#include "nizk.h"
#include <gmp.h>
#include <stdint.h>
#include "puzzle.h"
#include "params.h"

class VTD_crs_t {
public:
    mpz_t crs_range;
    LHP_param_t params;

    VTD_crs_t();
    ~VTD_crs_t();
};

void Setup_VTD(VTD_crs_t& crs, uint64_t lambda, uint64_t T);
void Commit_VTD(LHP_Puzzle* Z, Proof* proofs, mpz_t H, mpz_t* w_shares, mpz_t* H_shares, mpz_t w, VTD_crs_t& crs, mpz_t g, mpz_t prime, int n, int t, uint64_t lambda);
void Verify_VTD(VTD_crs_t& crs, LHP_Puzzle* Z, mpz_t H, mpz_t* H_shares, mpz_t* w_shares, Proof* proofs, mpz_t g, mpz_t prime, int n, int t);
#endif