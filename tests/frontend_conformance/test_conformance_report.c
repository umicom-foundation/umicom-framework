/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_conformance_report.c
 *
 * PURPOSE:
 *   Focused regression coverage for aggregate result collection, counts and score calculation for one frontend run.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/conformance_report.h"
#include "umicom/frontend/conformance/conformance_result.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcConformanceReport p; UmiFcConformanceResult r={"a",UMI_FC_PASS,0U,1.0}; umi_fc_conformance_report_init(&p); CHECK(umi_fc_conformance_report_add(&p,&r)==UMI_STATUS_OK); r.outcome=UMI_FC_DEGRADED; r.score=0.5; CHECK(umi_fc_conformance_report_add(&p,&r)==UMI_STATUS_OK); CHECK(p.passed==1U&&p.degraded==1U); CHECK(umi_fc_conformance_report_score(&p)>0.74);
    return 0;
}
