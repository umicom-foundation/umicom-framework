/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/restore_report.c
 *
 * PURPOSE:
 *   Initialize durable restore evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/restore_report.h"

#include <string.h>

void umi_ai_developer_restore_report_init(
    UmiAiDeveloperRestoreReport *report)
{
    if (report == NULL) return;

    (void)memset(report, 0, sizeof(*report));
    report->status = UMI_STATUS_OK;
}
