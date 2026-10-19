/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/qualification_history.c
 * Purpose: Bind persisted qualification summaries to their original run identity.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/distribution/runtime/qualification_history.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiReleaseQualificationJobIdentity(const UmiReleaseQualification *run,
    UmiJobIdentity *outIdentity)
{
    if (run == NULL || outIdentity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiReleaseQualificationIdentity identity;
    UmiStatus status = UmiReleaseQualificationGetIdentity(run, &identity);
    if (status != UMI_STATUS_OK) return status;
    UmiJobIdentity result = {0};
    memcpy(result.subject, identity.execution, sizeof(result.subject));
    memcpy(result.configuration, identity.plan, sizeof(result.configuration));
    memcpy(result.inputs, UmiReleaseQualificationSourceFingerprint(run), sizeof(result.inputs));
    status = UmiJobIdentityValidate(&result);
    if (status == UMI_STATUS_OK) *outIdentity = result;
    return status;
}
UmiStatus UmiReleaseQualificationHistoryBegin(const UmiReleaseQualification *run,
    UmiJobHistory *history, const char *label, UmiJobHistoryEntry *outEntry)
{
    if (history == NULL || outEntry == NULL || label == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiReleaseQualificationSummary summary;
    UmiStatus status = UmiReleaseQualificationDescribe(run, &summary);
    if (status != UMI_STATUS_OK) return status;
    if (summary.closed || summary.executionStatus != UMI_STATUS_OK || summary.buildRecorded || summary.recorded != 0U) return UMI_STATUS_INVALID_STATE;
    UmiJobIdentity identity;
    status = UmiReleaseQualificationJobIdentity(run, &identity);
    if (status != UMI_STATUS_OK) return status;
    return UmiJobHistoryBeginIdentified(history, "qualification.scoped", label, 3U, &identity, outEntry);
}
UmiStatus UmiReleaseQualificationHistoryPublish(const UmiReleaseQualification *run,
    UmiJobHistory *history, uint64_t entryId, uint64_t expectedRevision, UmiJobHistoryEntry *outEntry)
{
    if (run == NULL || history == NULL || outEntry == NULL || entryId == 0U || expectedRevision == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiJobIdentity identity;
    UmiStatus status = UmiReleaseQualificationJobIdentity(run, &identity);
    if (status != UMI_STATUS_OK) return status;
    UmiReleaseQualificationSummary summary;
    status = UmiReleaseQualificationDescribe(run, &summary);
    if (status != UMI_STATUS_OK) return status;
    /* Capture and update have separate short transactions. The revision check in
     * Update closes that race; an intervening writer is never silently replaced. */
    UmiJobHistorySnapshot *snapshot = calloc(1U, sizeof(*snapshot));
    if (snapshot == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiJobHistoryCapture(history, snapshot);
    UmiJobHistoryEntry previous = {0};
    if (status == UMI_STATUS_OK) {
        status = UMI_STATUS_NOT_FOUND;
        for (size_t i = 0U; i < snapshot->count; ++i)
            if (snapshot->entries[i].id == entryId) {
                previous = snapshot->entries[i];
                status = UMI_STATUS_OK;
                break;
            }
    }
    free(snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (previous.revision != expectedRevision || previous.total_steps != 3U ||
        strcmp(previous.kind, "qualification.scoped") != 0 ||
        UmiJobIdentityCompare(&previous.identity, &identity) != UMI_JOB_IDENTITY_SAME_RECORDED_INPUTS)
        return UMI_STATUS_INVALID_STATE;
    unsigned completed = 0U;
    if (summary.buildRecorded) {
        completed = 1U;
        if (summary.missing == 0U) {
            completed = 2U;
            if (summary.closed) completed = 3U;
        }
    }
    UmiJobHistoryState state = UMI_JOB_HISTORY_RUNNING;
    UmiStatus result = UMI_STATUS_OK;
    if (summary.closed) {
        state = summary.readyForOwnerReview ? UMI_JOB_HISTORY_SUCCEEDED : UMI_JOB_HISTORY_FAILED;
        if (!summary.readyForOwnerReview) result = UMI_STATUS_INVALID_STATE;
        /* Preserve the actual orchestration cause, including late cancellation. */
        if (summary.executionStatus != UMI_STATUS_OK) result = summary.executionStatus;
        if (summary.executionStatus == UMI_STATUS_CANCELLED) state = UMI_JOB_HISTORY_CANCELLED;
    }
    if (previous.state == state && previous.completed_steps == completed && previous.result == result) {
        *outEntry = previous;
        return UMI_STATUS_OK;
    }
    if (previous.state == UMI_JOB_HISTORY_PREPARED && state == UMI_JOB_HISTORY_SUCCEEDED)
        return UMI_STATUS_INVALID_STATE;
    return UmiJobHistoryUpdate(history, entryId, expectedRevision, state, completed, result, outEntry);
}
