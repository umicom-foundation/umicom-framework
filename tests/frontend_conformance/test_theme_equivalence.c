/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_theme_equivalence.c
 *
 * PURPOSE:
 *   Focused regression coverage for semantic theme-token equivalence scoring and required-token coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/theme_equivalence.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcThemeEquivalence e={10U,9U,1U}; CHECK(umi_fc_theme_equivalence_evaluate(&e)>0.89);
    return 0;
}
