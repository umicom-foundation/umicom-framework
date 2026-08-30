/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_surface_support.c
 *
 * PURPOSE:
 *   Focused regression coverage for support scoring and blocker detection for a renderer against one semantic surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/surface_support.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcSurfaceSupport x; umi_fc_surface_support_init(&x); x.required=UINT64_C(3); x.supported=UINT64_C(1); CHECK(!umi_fc_surface_support_satisfied(&x)); CHECK(umi_fc_surface_support_score(&x) > 0.49); x.supported=UINT64_C(3); CHECK(umi_fc_surface_support_satisfied(&x));
    return 0;
}
