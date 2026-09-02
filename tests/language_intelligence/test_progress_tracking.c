/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_progress_tracking.c
 * PURPOSE: Focused regression test for progress tracking.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/progress_tracking.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceProgressTracking progress;
    umi_language_intelligence_progress_tracking_init(&progress, "index", 100U);
    CHECK(umi_language_intelligence_progress_tracking_advance(&progress, 25U) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_progress_tracking_basis_points(&progress) == 2500U);
    umi_language_intelligence_progress_tracking_finish(&progress);
    CHECK(progress.state == UMI_LANGUAGE_INTELLIGENCE_READY);
    return 0;
}
