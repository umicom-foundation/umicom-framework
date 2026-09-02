/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_portable_plan.c
 *
 * PURPOSE:
 *   Focused regression coverage for self-contained portable bundle policy and relocatability validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/portable_plan.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrPortablePlan p; umi_dr_portable_plan_init(&p); p.required_steps=2U; CHECK(umi_dr_portable_plan_complete_step(&p)==UMI_STATUS_OK); CHECK(!umi_dr_portable_plan_ready(&p)); CHECK(umi_dr_portable_plan_complete_step(&p)==UMI_STATUS_OK); CHECK(umi_dr_portable_plan_ready(&p));
    return 0;
}
