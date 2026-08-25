/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_layout_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for toolkit-neutral layout structure expectations used by frontend equivalence checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/layout_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcLayoutContract a={3U,1U,1U,0U,3U},b={3U,2U,1U,0U,3U}; CHECK(umi_fc_layout_contract_satisfied(&a,&b)); CHECK(umi_fc_layout_contract_score(&a,&b)==1.0);
    return 0;
}
