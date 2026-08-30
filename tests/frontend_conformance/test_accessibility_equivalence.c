/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_accessibility_equivalence.c
 *
 * PURPOSE:
 *   Focused regression coverage for accessibility capability coverage and blocker detection across frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/accessibility_equivalence.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcAccessibilityEquivalence e={7U,3U}; CHECK(umi_fc_accessibility_equivalence_evaluate(&e)>0.66); CHECK(umi_fc_accessibility_equivalence_has_blocker(&e));
    return 0;
}
