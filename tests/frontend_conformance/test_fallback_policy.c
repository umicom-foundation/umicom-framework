/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_fallback_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for canonical fallback selection for unavailable frontend capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/fallback_policy.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    CHECK(umi_fc_fallback_policy_for(UMI_FC_CAP_FLOAT,UMI_FC_FRONTEND_NATIVE_WEB)==UMI_FC_FALLBACK_TAB); CHECK(umi_fc_fallback_policy_preserves_semantics(UMI_FC_FALLBACK_TAB));
    return 0;
}
