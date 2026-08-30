/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_layout_adaptation.c
 *
 * PURPOSE:
 *   Focused regression coverage for explicit desktop-to-web and constrained-viewport layout adaptation planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/layout_adaptation.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcLayoutAdaptation a=umi_fc_layout_adaptation_plan(true,false,false); CHECK(a.kind==UMI_FC_LAYOUT_TAB); CHECK(a.lost_capabilities!=0U); CHECK(a.user_visible);
    return 0;
}
