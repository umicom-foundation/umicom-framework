/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_runtime_probe.c
 *
 * PURPOSE:
 *   Focused regression coverage for deterministic probe snapshots describing detected host runtime properties.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_probe.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrRuntimeProbe value; umi_dr_runtime_probe_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"host") == UMI_STATUS_OK); value.memory_mb=16384U; CHECK(umi_dr_runtime_probe_valid(&value)); CHECK(umi_dr_runtime_probe_fingerprint(&value) != 0U);
    return 0;
}
