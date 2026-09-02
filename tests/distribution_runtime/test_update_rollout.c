/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_update_rollout.c
 *
 * PURPOSE:
 *   Focused regression coverage for deterministic staged-rollout cohort selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/update_rollout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    CHECK(umi_dr_update_rollout_bucket("machine")<100U); CHECK(umi_dr_update_rollout_selected("machine",100U)); CHECK(!umi_dr_update_rollout_selected("machine",0U));
    return 0;
}
