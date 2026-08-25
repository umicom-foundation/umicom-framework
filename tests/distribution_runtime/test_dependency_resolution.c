/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_dependency_resolution.c
 *
 * PURPOSE:
 *   Focused regression coverage for bounded dependency graph with cycle detection and topological readiness.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/dependency_resolution.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrDependencyResolution g; umi_dr_dependency_resolution_init(&g,3U); CHECK(umi_dr_dependency_resolution_add(&g,0U,1U)==UMI_STATUS_OK); CHECK(umi_dr_dependency_resolution_add(&g,1U,2U)==UMI_STATUS_OK); CHECK(!umi_dr_dependency_resolution_has_cycle(&g)); CHECK(umi_dr_dependency_resolution_add(&g,2U,0U)==UMI_STATUS_OK); CHECK(umi_dr_dependency_resolution_has_cycle(&g));
    return 0;
}
