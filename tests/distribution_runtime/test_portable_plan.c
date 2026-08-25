/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_portable_plan.c
 *
 * PURPOSE:
 *   Focused regression coverage for self-contained portable bundle policy and relocatability validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/portable_plan.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrPortablePlan p; umi_dr_portable_plan_init(&p); p.required_steps=2U; CHECK(umi_dr_portable_plan_complete_step(&p)==UMI_STATUS_OK); CHECK(!umi_dr_portable_plan_ready(&p)); CHECK(umi_dr_portable_plan_complete_step(&p)==UMI_STATUS_OK); CHECK(umi_dr_portable_plan_ready(&p));
    return 0;
}
