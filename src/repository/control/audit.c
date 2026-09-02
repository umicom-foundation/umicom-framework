/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/audit.c
 *
 * PURPOSE:
 *   Retain a bounded in-memory audit trail of repository-control events.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/audit.h"

#include <string.h>

/*
 * Initialise repository audit from caller-provided values so later operations receive a
 * known state.
 */
void umi_repository_audit_init(UmiRepositoryAudit *audit)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (audit == NULL) return;
    (void)memset(audit, 0, sizeof(*audit));
    audit->next_sequence = 1U;
}

/* Add repository audit only after its inputs and available capacity have been checked. */
UmiStatus umi_repository_audit_append(
    UmiRepositoryAudit *audit,
    UmiRepositoryControlState state,
    UmiStatus status,
    const char *path,
    const char *message)
{
    UmiStatus event_status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (audit == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (audit->count >= UMI_REPOSITORY_CONTROL_EVENT_CAPACITY) {
        (void)memmove(
            &audit->events[0],
            &audit->events[1],
            (UMI_REPOSITORY_CONTROL_EVENT_CAPACITY - 1U) *
                sizeof(audit->events[0]));
        audit->count = UMI_REPOSITORY_CONTROL_EVENT_CAPACITY - 1U;
    }
    event_status = umi_repository_event_init(
        &audit->events[audit->count],
        audit->next_sequence,
        state,
        status,
        path,
        message);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (event_status != UMI_STATUS_OK) return event_status;
    audit->count += 1U;
    audit->next_sequence += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the repository audit latest operation used by this module and its client
 * applications.
 */
const UmiRepositoryControlEvent *umi_repository_audit_latest(
    const UmiRepositoryAudit *audit)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (audit == NULL || audit->count == 0U) return NULL;
    return &audit->events[audit->count - 1U];
}
