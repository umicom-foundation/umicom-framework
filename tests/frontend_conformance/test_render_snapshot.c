/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_render_snapshot.c
 *
 * PURPOSE:
 *   Focused regression coverage for normalised semantic render snapshot used for frontend regression testing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/render_snapshot.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcRenderSnapshot s; CHECK(umi_fc_render_snapshot_make("workbench",10U,100U,4U,123U,1U,&s)==UMI_STATUS_OK); CHECK(s.node_count==10U&&s.semantic_fingerprint==123U);
    return 0;
}
