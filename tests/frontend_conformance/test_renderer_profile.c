/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_renderer_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for renderer identity, capability and policy metadata used by conformance evaluation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/renderer_profile.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcRendererProfile p; CHECK(umi_fc_renderer_profile_make("gtk4",UMI_FC_FRONTEND_GTK4,UINT64_C(7),1U,&p)==UMI_STATUS_OK); CHECK(p.kind==UMI_FC_FRONTEND_GTK4); CHECK(umi_fc_renderer_profile_validate(&p)==UMI_STATUS_OK);
    return 0;
}
