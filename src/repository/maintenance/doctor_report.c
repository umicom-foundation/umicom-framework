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

/*
 * Initialise repository doctor report from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_doctor_report_init(UmiRepositoryDoctorReport *report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL) return;
    (void)memset(report, 0, sizeof(*report));
    umi_repository_status_summary_init(&report->summary);
    umi_repository_doctor_issue_list_init(&report->issues);
}

/*
 * Provide the repository doctor report finalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_doctor_report_finalize(UmiRepositoryDoctorReport *report)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    report->warnings = 0U; report->errors = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < report->issues.count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (report->issues.items[index].severity == UMI_REPOSITORY_DOCTOR_WARNING) report->warnings += 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (report->issues.items[index].severity == UMI_REPOSITORY_DOCTOR_ERROR) report->errors += 1U;
    }
    report->healthy = report->errors == 0U;
    return UMI_STATUS_OK;
}
