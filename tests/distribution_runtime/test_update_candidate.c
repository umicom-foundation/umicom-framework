/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_update_candidate.c
 *
 * PURPOSE:
 *   Focused regression coverage for installed-versus-available update candidate classification.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/update_candidate.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    CHECK(umi_dr_update_candidate_classify((UmiDrVersion){1,0,0},(UmiDrVersion){1,1,0})==UMI_DR_UPDATE_UPGRADE); CHECK(umi_dr_update_candidate_classify((UmiDrVersion){2,0,0},(UmiDrVersion){1,0,0})==UMI_DR_UPDATE_DOWNGRADE);
    return 0;
}
