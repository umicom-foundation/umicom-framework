/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/report.c
 *
 * PURPOSE:
 *   Build product-level journey completion and release-readiness reports from evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/journey/report.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_application_journey_report_build(
    const UmiApplicationJourneyCatalogue *catalogue,
    const UmiApplicationJourneyEvidenceRegistry *evidence,
    const char *application_id,
    UmiApplicationJourneyReport *out_report)
{
    size_t journey_index;
    size_t blocking_pending = 0U;
    if (catalogue == NULL || evidence == NULL || application_id == NULL ||
        application_id[0] == '\0' || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    (void)snprintf(out_report->application_id,
                   sizeof(out_report->application_id), "%s", application_id);
    for (journey_index = 0U; journey_index < catalogue->count; ++journey_index) {
        const UmiApplicationJourney *journey = catalogue->entries[journey_index];
        size_t step_index;
        if (journey == NULL ||
            strcmp(journey->application_id, application_id) != 0)
            continue;
        out_report->journey_count++;
        if (journey->blocks_release) out_report->blocking_journey_count++;
        for (step_index = 0U; step_index < journey->step_count; ++step_index) {
            const UmiApplicationJourneyEvidenceRecord *record =
                umi_application_journey_evidence_find(
                    evidence, journey->journey_id,
                    journey->steps[step_index].step_id);
            out_report->step_count++;
            if (record == NULL ||
                record->result == UMI_APPLICATION_JOURNEY_PENDING) {
                out_report->pending_step_count++;
                if (journey->blocks_release) blocking_pending++;
            } else if (record->result == UMI_APPLICATION_JOURNEY_PASSED) {
                out_report->passed_step_count++;
            } else {
                out_report->failed_step_count++;
                if (journey->blocks_release) blocking_pending++;
            }
        }
    }
    if (out_report->journey_count == 0U) return UMI_STATUS_NOT_FOUND;
    if (out_report->step_count > 0U)
        out_report->completion_percent = (unsigned)(
            out_report->passed_step_count * 100U / out_report->step_count);
    out_report->release_ready =
        blocking_pending == 0U && out_report->failed_step_count == 0U;
    return UMI_STATUS_OK;
}

