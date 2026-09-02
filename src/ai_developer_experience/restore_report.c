/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/restore_report.c
 *
 * PURPOSE:
 *   Initialize durable restore evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/restore_report.h"

#include <string.h>

/*
 * Initialise ai developer restore report from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_developer_restore_report_init(
    UmiAiDeveloperRestoreReport *report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL) return;

    (void)memset(report, 0, sizeof(*report));
    report->status = UMI_STATUS_OK;
}
