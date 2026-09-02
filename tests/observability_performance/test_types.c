/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/observability_performance/test_types.c
 *
 * PURPOSE:
 *   Implement the test types behavior for
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
#include "umicom/observability/performance/types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    char text[8];
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_performance_copy_text(text, sizeof(text), "abc") != UMI_STATUS_OK) return 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_performance_id_valid("sample")) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_clamp_unit(2.0) != 1.0) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_percent_delta(120.0, 100.0) != 20.0) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_safe_ratio(10.0, 2.0) != 5.0) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_hash_text("abc") == 0U) return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_performance_severity_from_thresholds(95.0, 80.0, 90.0) != UMI_PERFORMANCE_SEVERITY_CRITICAL) return 7;
    puts("types: ok");
    return 0;
}
