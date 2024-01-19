#include "gmp.h"
#include "vtd.h"

VTD_crs_t::VTD_crs_t() {
    mpz_init(crs_range);
    LHP_param_t params;
}

VTD_crs_t::~VTD_crs_t() {
    mpz_clear(crs_range);
}