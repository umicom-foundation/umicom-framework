/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/auto_configuration_report.c
 *
 * PURPOSE:
 *   Record applied, skipped and failed auto-configuration outcomes.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/auto_configuration_report.h"


#include <string.h>
void umi_bootstrap_auto_configuration_report_init(
    UmiBootstrapAutoConfigurationReport *report) {
    if (report != NULL) memset(report, 0, sizeof(*report));
}
UmiStatus umi_bootstrap_auto_configuration_report_record(
    UmiBootstrapAutoConfigurationReport *report,
    UmiBootstrapStageState outcome,
    const char *message) {
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    switch (outcome) {
        case UMI_BOOTSTRAP_STAGE_COMPLETE: ++report->applied; break;
        case UMI_BOOTSTRAP_STAGE_SKIPPED: ++report->skipped; break;
        case UMI_BOOTSTRAP_STAGE_FAILED: ++report->failed; break;
        default: return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (message != NULL)
        (void)umi_bootstrap_copy_text(report->last_message,
            sizeof(report->last_message), message);
    return UMI_STATUS_OK;
}
