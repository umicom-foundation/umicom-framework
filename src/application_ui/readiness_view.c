/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_ui/readiness_view.c
 *
 * PURPOSE:
 *   Build deterministic readiness rows for any frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application_ui/readiness_view.h"

/*
 * Provide the application ui readiness rows operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_ui_readiness_rows(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationUiReadinessRow rows[UMI_APPLICATION_UI_READINESS_ROW_COUNT])
{
    UmiApplicationReadinessReport report;
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (rows == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_application_readiness_report(experience, &report);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    rows[0] = (UmiApplicationUiReadinessRow){"planned", report.planned_count};
    rows[1] = (UmiApplicationUiReadinessRow){"foundation", report.foundation_count};
    rows[2] = (UmiApplicationUiReadinessRow){"implemented", report.implemented_count};
    rows[3] = (UmiApplicationUiReadinessRow){"verified", report.verified_count};
    rows[4] = (UmiApplicationUiReadinessRow){"open-p0", report.p0_open_count};
    rows[5] = (UmiApplicationUiReadinessRow){"open-p1", report.p1_open_count};
    return UMI_STATUS_OK;
}
