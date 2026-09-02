/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_runtime_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for named runtime profiles combining platform, architecture and capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_profile.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrRuntimeProfile value; umi_dr_runtime_profile_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"linux-x64") == UMI_STATUS_OK); CHECK(umi_dr_runtime_profile_valid(&value)); CHECK(umi_dr_runtime_profile_fingerprint(&value) != 0U);
    return 0;
}
