/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/bootstrap_report.c
 *
 * PURPOSE:
 *   Implement the bootstrap report behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/bootstrap_report.c
 *
 * PURPOSE:
 *   Summarise bootstrap completion, skip and failure outcomes.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/bootstrap_report.h"


#include <string.h>
/*
 * Initialise bootstrap report from caller-provided values so later operations receive a
 * known state.
 */
void umi_bootstrap_report_init(UmiBootstrapReport *report) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report != NULL) memset(report, 0, sizeof(*report));
}
/*
 * Provide the bootstrap report record operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_report_record(UmiBootstrapReport *report,
                                      const UmiBootstrapStage *stage,
                                      const char *message) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL || stage == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Select the behaviour associated with the requested command or state value. */
    switch (stage->state) {
        case UMI_BOOTSTRAP_STAGE_COMPLETE: ++report->completed; break;
        case UMI_BOOTSTRAP_STAGE_SKIPPED: ++report->skipped; break;
        case UMI_BOOTSTRAP_STAGE_FAILED: ++report->failed; break;
        default: return UMI_STATUS_INVALID_STATE;
    }
    (void)umi_bootstrap_copy_text(report->last_stage, sizeof(report->last_stage), stage->stage_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (message != NULL)
        (void)umi_bootstrap_copy_text(report->message, sizeof(report->message), message);
    return UMI_STATUS_OK;
}
