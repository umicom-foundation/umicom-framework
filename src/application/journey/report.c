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
/*
 * Provide the application journey report build operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_journey_report_build(
    const UmiApplicationJourneyCatalogue *catalogue,
    const UmiApplicationJourneyEvidenceRegistry *evidence,
    const char *application_id,
    UmiApplicationJourneyReport *out_report)
{
    size_t journey_index;
    size_t blocking_pending = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || evidence == NULL || application_id == NULL ||
        application_id[0] == '\0' || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    (void)snprintf(out_report->application_id,
                   sizeof(out_report->application_id), "%s", application_id);
    /* Visit each bounded item once so every record receives the same rule. */
    for (journey_index = 0U; journey_index < catalogue->count; ++journey_index) {
        const UmiApplicationJourney *journey = catalogue->entries[journey_index];
        size_t step_index;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (journey == NULL ||
            strcmp(journey->application_id, application_id) != 0)
            continue;
        out_report->journey_count++;
        /* Apply this branch only when its contract condition is satisfied. */
        if (journey->blocks_release) out_report->blocking_journey_count++;
        /* Visit each bounded item once so every record receives the same rule. */
        for (step_index = 0U; step_index < journey->step_count; ++step_index) {
            const UmiApplicationJourneyEvidenceRecord *record =
                umi_application_journey_evidence_find(
                    evidence, journey->journey_id,
                    journey->steps[step_index].step_id);
            out_report->step_count++;
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (record == NULL ||
                record->result == UMI_APPLICATION_JOURNEY_PENDING) {
                out_report->pending_step_count++;
                /* Apply this branch only when its contract condition is satisfied. */
                if (journey->blocks_release) blocking_pending++;
            } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (record->result == UMI_APPLICATION_JOURNEY_PASSED) {
                out_report->passed_step_count++;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                out_report->failed_step_count++;
                /* Apply this branch only when its contract condition is satisfied. */
                if (journey->blocks_release) blocking_pending++;
            }
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (out_report->journey_count == 0U) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_report->step_count > 0U)
        out_report->completion_percent = (unsigned)(
            out_report->passed_step_count * 100U / out_report->step_count);
    out_report->release_ready =
        blocking_pending == 0U && out_report->failed_step_count == 0U;
    return UMI_STATUS_OK;
}

