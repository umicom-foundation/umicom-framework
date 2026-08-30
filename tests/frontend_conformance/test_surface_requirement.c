/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_surface_requirement.c
 *
 * PURPOSE:
 *   Focused regression coverage for required and optional semantic capability requirements for a Framework UI surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/surface_requirement.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcSurfaceRequirement r; CHECK(umi_fc_surface_requirement_make("editor",3U,4U,false,&r)==UMI_STATUS_OK); CHECK(umi_fc_surface_requirement_missing(&r,1U)==2U);
    return 0;
}
