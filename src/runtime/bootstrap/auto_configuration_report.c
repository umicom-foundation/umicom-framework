/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/auto_configuration_report.c
 *
 * PURPOSE:
 *   Implement the auto configuration report behavior for
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
 * File: src/runtime/bootstrap/auto_configuration_report.c
 *
 * PURPOSE:
 *   Record applied, skipped and failed auto-configuration outcomes.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/auto_configuration_report.h"


#include <string.h>
/*
 * Initialise bootstrap auto configuration report from caller-provided values so later
 * operations receive a known state.
 */
void umi_bootstrap_auto_configuration_report_init(
    UmiBootstrapAutoConfigurationReport *report) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report != NULL) memset(report, 0, sizeof(*report));
}
/*
 * Provide the bootstrap auto configuration report record operation used by this module and
 * its client applications.
 */
UmiStatus umi_bootstrap_auto_configuration_report_record(
    UmiBootstrapAutoConfigurationReport *report,
    UmiBootstrapStageState outcome,
    const char *message) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Select the behaviour associated with the requested command or state value. */
    switch (outcome) {
        case UMI_BOOTSTRAP_STAGE_COMPLETE: ++report->applied; break;
        case UMI_BOOTSTRAP_STAGE_SKIPPED: ++report->skipped; break;
        case UMI_BOOTSTRAP_STAGE_FAILED: ++report->failed; break;
        default: return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (message != NULL)
        (void)umi_bootstrap_copy_text(report->last_message,
            sizeof(report->last_message), message);
    return UMI_STATUS_OK;
}
