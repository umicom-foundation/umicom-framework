/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_crash_signature.c
 *
 * PURPOSE:
 *   Implement the test crash signature behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/observability/performance/crash_signature.h"

int main(void) {
    UmiPerformanceCrashSignature left;
    UmiPerformanceCrashSignature right;
    if (umi_performance_crash_signature_init(&left, "crash_signature", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_crash_signature_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_crash_signature_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_crash_signature_fingerprint("evidence") == 0U) return 4;
    if (umi_performance_crash_signature_init(&right, "crash_signature", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_crash_signature_same_identity(&left, &right)) return 6;
    puts("crash_signature: ok");
    return 0;
}
