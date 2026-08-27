/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/journey/runtime.c
 *
 * PURPOSE:
 *   Run journey steps in order and preserve critical failure evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/journey/runtime.h"
#include <string.h>
UmiStatus umi_application_journey_runtime_begin(
    UmiApplicationJourneyRuntime *runtime,
    const UmiApplicationJourney *journey,
    UmiApplicationJourneyEvidenceRegistry *evidence)
{
    UmiStatus status;
    if (runtime == NULL || evidence == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_journey_validate(journey);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(runtime, 0, sizeof(*runtime));
    runtime->journey = journey;
    runtime->evidence = evidence;
    runtime->result = UMI_APPLICATION_JOURNEY_PENDING;
    runtime->active = 1;
    return UMI_STATUS_OK;
}
const UmiApplicationJourneyStep *umi_application_journey_runtime_current(
    const UmiApplicationJourneyRuntime *runtime)
{
    if (runtime == NULL || !runtime->active || runtime->complete ||
        runtime->journey == NULL ||
        runtime->next_step_index >= runtime->journey->step_count)
        return NULL;
    return &runtime->journey->steps[runtime->next_step_index];
}
UmiStatus umi_application_journey_runtime_record(
    UmiApplicationJourneyRuntime *runtime,
    UmiApplicationJourneyResult result,
    const char *note,
    uint64_t timestamp_ms)
{
    const UmiApplicationJourneyStep *step =
        umi_application_journey_runtime_current(runtime);
    UmiStatus status;
    if (step == NULL ||
        (result != UMI_APPLICATION_JOURNEY_PASSED &&
         result != UMI_APPLICATION_JOURNEY_FAILED))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_journey_evidence_record(
        runtime->evidence, runtime->journey->journey_id, step->step_id,
        result, note, timestamp_ms);
    if (status != UMI_STATUS_OK) return status;
    if (result == UMI_APPLICATION_JOURNEY_FAILED) {
        runtime->failed_step_count++;
        if (step->critical) {
            runtime->result = UMI_APPLICATION_JOURNEY_BLOCKED;
            runtime->active = 0;
            return UMI_STATUS_OK;
        }
    } else {
        runtime->passed_step_count++;
    }
    runtime->next_step_index++;
    if (runtime->next_step_index >= runtime->journey->step_count) {
        runtime->complete = 1;
        runtime->active = 0;
        runtime->result = runtime->failed_step_count == 0U
            ? UMI_APPLICATION_JOURNEY_PASSED
            : UMI_APPLICATION_JOURNEY_FAILED;
    }
    return UMI_STATUS_OK;
}

