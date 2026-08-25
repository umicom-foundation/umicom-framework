/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/bootstrap_report.c
 *
 * PURPOSE:
 *   Summarise bootstrap completion, skip and failure outcomes.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/bootstrap_report.h"


#include <string.h>
void umi_bootstrap_report_init(UmiBootstrapReport *report) {
    if (report != NULL) memset(report, 0, sizeof(*report));
}
UmiStatus umi_bootstrap_report_record(UmiBootstrapReport *report,
                                      const UmiBootstrapStage *stage,
                                      const char *message) {
    if (report == NULL || stage == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    switch (stage->state) {
        case UMI_BOOTSTRAP_STAGE_COMPLETE: ++report->completed; break;
        case UMI_BOOTSTRAP_STAGE_SKIPPED: ++report->skipped; break;
        case UMI_BOOTSTRAP_STAGE_FAILED: ++report->failed; break;
        default: return UMI_STATUS_INVALID_STATE;
    }
    (void)umi_bootstrap_copy_text(report->last_stage, sizeof(report->last_stage), stage->stage_id);
    if (message != NULL)
        (void)umi_bootstrap_copy_text(report->message, sizeof(report->message), message);
    return UMI_STATUS_OK;
}
