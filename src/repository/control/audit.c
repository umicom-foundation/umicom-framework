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

void umi_repository_audit_init(UmiRepositoryAudit *audit)
{
    if (audit == NULL) return;
    (void)memset(audit, 0, sizeof(*audit));
    audit->next_sequence = 1U;
}

UmiStatus umi_repository_audit_append(
    UmiRepositoryAudit *audit,
    UmiRepositoryControlState state,
    UmiStatus status,
    const char *path,
    const char *message)
{
    UmiStatus event_status;
    if (audit == NULL) return UMI_STATUS_INVALID_ARGUMENT;
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
    if (event_status != UMI_STATUS_OK) return event_status;
    audit->count += 1U;
    audit->next_sequence += 1U;
    return UMI_STATUS_OK;
}

const UmiRepositoryControlEvent *umi_repository_audit_latest(
    const UmiRepositoryAudit *audit)
{
    if (audit == NULL || audit->count == 0U) return NULL;
    return &audit->events[audit->count - 1U];
}
