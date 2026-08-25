/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_distribution_session.c
 *
 * PURPOSE:
 *   Focused regression coverage for top-level distribution planning session aggregating runtime/package/update readiness.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/distribution_session.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrDistributionSession s; umi_dr_distribution_session_init(&s,"release"); s.runtime_ready=s.package_ready=s.release_gate_passed=s.deployment_ready=true; CHECK(umi_dr_distribution_session_ready(&s)); CHECK(umi_dr_distribution_session_commit(&s)==UMI_STATUS_OK); CHECK(s.state==UMI_DR_STATE_COMMITTED && s.revision==1U);
    return 0;
}
