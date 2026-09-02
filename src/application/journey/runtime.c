/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/runtime.c
 *
 * PURPOSE:
 *   Run journey steps in order and preserve critical failure evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/journey/runtime.h"
#include <string.h>
/*
 * Provide the application journey runtime begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_journey_runtime_begin(
    UmiApplicationJourneyRuntime *runtime,
    const UmiApplicationJourney *journey,
    UmiApplicationJourneyEvidenceRegistry *evidence)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || evidence == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_journey_validate(journey);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(runtime, 0, sizeof(*runtime));
    runtime->journey = journey;
    runtime->evidence = evidence;
    runtime->result = UMI_APPLICATION_JOURNEY_PENDING;
    runtime->active = 1;
    return UMI_STATUS_OK;
}
/*
 * Provide the application journey runtime current operation used by this module and its
 * client applications.
 */
const UmiApplicationJourneyStep *umi_application_journey_runtime_current(
    const UmiApplicationJourneyRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || !runtime->active || runtime->complete ||
        runtime->journey == NULL ||
        runtime->next_step_index >= runtime->journey->step_count)
        return NULL;
    return &runtime->journey->steps[runtime->next_step_index];
}
/*
 * Provide the application journey runtime record operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_journey_runtime_record(
    UmiApplicationJourneyRuntime *runtime,
    UmiApplicationJourneyResult result,
    const char *note,
    uint64_t timestamp_ms)
{
    const UmiApplicationJourneyStep *step =
        umi_application_journey_runtime_current(runtime);
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (step == NULL ||
        (result != UMI_APPLICATION_JOURNEY_PASSED &&
         result != UMI_APPLICATION_JOURNEY_FAILED))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_journey_evidence_record(
        runtime->evidence, runtime->journey->journey_id, step->step_id,
        result, note, timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result == UMI_APPLICATION_JOURNEY_FAILED) {
        runtime->failed_step_count++;
        /* Apply this branch only when its contract condition is satisfied. */
        if (step->critical) {
            runtime->result = UMI_APPLICATION_JOURNEY_BLOCKED;
            runtime->active = 0;
            return UMI_STATUS_OK;
        }
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        runtime->passed_step_count++;
    }
    runtime->next_step_index++;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (runtime->next_step_index >= runtime->journey->step_count) {
        runtime->complete = 1;
        runtime->active = 0;
        runtime->result = runtime->failed_step_count == 0U
            ? UMI_APPLICATION_JOURNEY_PASSED
            : UMI_APPLICATION_JOURNEY_FAILED;
    }
    return UMI_STATUS_OK;
}

