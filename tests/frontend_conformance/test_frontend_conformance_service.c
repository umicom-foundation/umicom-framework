/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_frontend_conformance_service.c
 *
 * PURPOSE:
 *   Focused regression coverage for top-level Framework-owned service for renderer registration and conformance evaluation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/frontend_conformance_service.h"
#include "umicom/frontend/conformance/renderer_profile.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFcFrontendConformanceService s; UmiFcRendererProfile p; UmiFcFrontendHealth h; umi_fc_frontend_conformance_service_init(&s); CHECK(umi_fc_renderer_profile_make("gtk4",UMI_FC_FRONTEND_GTK4,7U,1U,&p)==UMI_STATUS_OK); CHECK(umi_fc_frontend_conformance_service_register(&s,&p)==UMI_STATUS_OK); CHECK(umi_fc_frontend_conformance_service_evaluate(&s,"gtk4",3U,0.96,0.98,0.95,0U,&h)==UMI_FC_PASS); CHECK(h.blockers==0U);
    return 0;
}
