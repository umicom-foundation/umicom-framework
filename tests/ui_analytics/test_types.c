/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_types.c
 *
 * PURPOSE:
 *   Validate types analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    char text[8];
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_analytics_copy_text(text, sizeof text, "metric") != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_analytics_number_valid(12.5)) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_analytics_compare(5.0, UMI_ANALYTICS_COMPARE_GT, 4.0)) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_analytics_hash_text("metric") == 0U) return 4;
    return 0;
}
