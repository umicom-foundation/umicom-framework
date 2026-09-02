/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_dependency_bundle.c
 *
 * PURPOSE:
 *   Focused regression coverage for resolved dependency set for one application release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/dependency_bundle.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrDependencyBundle value; umi_dr_dependency_bundle_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"deps")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.application_id,sizeof(value.application_id),"app")==UMI_STATUS_OK); CHECK(umi_dr_dependency_bundle_valid(&value)); CHECK(umi_dr_dependency_bundle_fingerprint(&value) != 0U);
    return 0;
}
