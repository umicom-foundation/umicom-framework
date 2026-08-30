/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/doctor_report.c
 *
 * PURPOSE:
 *   Implement doctor report severity aggregation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/doctor_report.h"

#include <string.h>

void umi_repository_doctor_report_init(UmiRepositoryDoctorReport *report)
{
    if (report == NULL) return;
    (void)memset(report, 0, sizeof(*report));
    umi_repository_status_summary_init(&report->summary);
    umi_repository_doctor_issue_list_init(&report->issues);
}

UmiStatus umi_repository_doctor_report_finalize(UmiRepositoryDoctorReport *report)
{
    size_t index;
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    report->warnings = 0U; report->errors = 0U;
    for (index = 0U; index < report->issues.count; ++index) {
        if (report->issues.items[index].severity == UMI_REPOSITORY_DOCTOR_WARNING) report->warnings += 1U;
        if (report->issues.items[index].severity == UMI_REPOSITORY_DOCTOR_ERROR) report->errors += 1U;
    }
    report->healthy = report->errors == 0U;
    return UMI_STATUS_OK;
}
