/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_retry_report.c
 *
 * PURPOSE:
 *   Derive retry selections from checked saved evidence without resuming old jobs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/qualification_retry.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include "qualification_parse_internal.h"
#include <string.h>

UmiStatus UmiReleaseQualificationPlanFromReport(const UmiReleaseInventory *inventory,
    const UmiReleaseSourceObservation *baseline,
    const UmiReleaseSourceObservation *current,
    const char *report, size_t length, const char *expectedReportSha256,
    const char *toolchain, UmiReleaseImpactPlan **outPlan,
    UmiQualificationRetrySummary *outSummary)
{
    if (inventory == NULL || current == NULL || report == NULL ||
        outPlan == NULL || *outPlan != NULL || outSummary == NULL ||
        !UmiQualificationDigestValid(expectedReportSha256) ||
        (toolchain != NULL && !UmiQualificationDigestValid(toolchain)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length >= UMI_QUALIFICATION_REPORT_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    char observed[65];
    UmiStatus status = UmiSha256Buffer(report, length, observed);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(observed, expectedReportSha256) != 0) return UMI_STATUS_INVALID_STATE;
    /* Reconstruct only for the duration of this read. The same strict parser
     * serves the saved-results view, preventing a more permissive retry path. */
    UmiReleaseQualification *previous = NULL;
    status = UmiQualificationParseRecord(report, length, &previous);
    if (status == UMI_STATUS_OK)
        status = UmiReleaseQualificationPlanNext(inventory, baseline, current,
            previous, toolchain, outPlan, outSummary);
    UmiReleaseQualificationDestroy(previous);
    return status;
}
