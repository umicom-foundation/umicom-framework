/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/lifecycle_evidence.c
 *
 * PURPOSE:
 *   Implement ordered bounded lifecycle evidence suitable for later Data Server persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/lifecycle_evidence.h"

#include <string.h>

/*
 * Initialise application lifecycle evidence from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_lifecycle_evidence_init(UmiApplicationLifecycleEvidence *evidence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (evidence == NULL) return;
    memset(evidence, 0, sizeof(*evidence));
    evidence->structure_size = sizeof(*evidence);
    evidence->next_sequence = 1U;
}

/*
 * Provide the application lifecycle evidence record operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_lifecycle_evidence_record(
    UmiApplicationLifecycleEvidence *evidence,
    UmiApplicationLifecyclePhase phase,
    UmiStatus status)
{
    UmiApplicationLifecycleEvent *event;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (evidence == NULL || phase < UMI_APPLICATION_LIFECYCLE_DISCOVERED ||
        phase > UMI_APPLICATION_LIFECYCLE_DESTROYED)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (evidence->event_count >= UMI_APPLICATION_RUNTIME_MAX_LIFECYCLE_EVENTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    event = &evidence->events[evidence->event_count++];
    event->sequence = evidence->next_sequence++;
    event->phase = phase;
    event->status = status;
    return UMI_STATUS_OK;
}

/*
 * Provide the application lifecycle evidence last operation used by this module and its
 * client applications.
 */
const UmiApplicationLifecycleEvent *umi_application_lifecycle_evidence_last(
    const UmiApplicationLifecycleEvidence *evidence)
{
    return evidence != NULL && evidence->event_count > 0U
        ? &evidence->events[evidence->event_count - 1U] : NULL;
}
