/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_archive_plan.c
 *
 * PURPOSE:
 *   Focused regression coverage for archive format, compression and deterministic-build policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/archive_plan.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrArchivePlan p; umi_dr_archive_plan_init(&p); p.required_steps=2U; CHECK(umi_dr_archive_plan_complete_step(&p)==UMI_STATUS_OK); CHECK(!umi_dr_archive_plan_ready(&p)); CHECK(umi_dr_archive_plan_complete_step(&p)==UMI_STATUS_OK); CHECK(umi_dr_archive_plan_ready(&p));
    return 0;
}
