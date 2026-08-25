/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_renderer_registry.c
 *
 * PURPOSE:
 *   Focused regression coverage for bounded registry of frontend renderer profiles with stable identity lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/renderer_registry.h"
#include "umicom/frontend/conformance/renderer_profile.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcRendererRegistry r; UmiFcRendererProfile p,q; umi_fc_renderer_registry_init(&r); CHECK(umi_fc_renderer_profile_make("web",UMI_FC_FRONTEND_NATIVE_WEB,3U,1U,&p)==UMI_STATUS_OK); CHECK(umi_fc_renderer_registry_upsert(&r,&p)==UMI_STATUS_OK); CHECK(umi_fc_renderer_registry_find(&r,"web",&q)==UMI_STATUS_OK); CHECK(q.kind==UMI_FC_FRONTEND_NATIVE_WEB);
    return 0;
}
