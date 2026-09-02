/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/event.c
 *
 * PURPOSE:
 *   Publish deterministic repository-control state transition events.
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
#include "umicom/repository/event.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise repository event from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_repository_event_init(
    UmiRepositoryControlEvent *event,
    uint64_t sequence,
    UmiRepositoryControlState state,
    UmiStatus status,
    const char *path,
    const char *message)
{
    UmiStatus copy_status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event == NULL || message == NULL || sequence == 0U ||
        state < UMI_REPOSITORY_CONTROL_IDLE ||
        state > UMI_REPOSITORY_CONTROL_FAILED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(event, 0, sizeof(*event));
    event->sequence = sequence;
    event->state = state;
    event->status = status;
    copy_status = copy_text(
        event->path, sizeof(event->path), path != NULL ? path : "");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (copy_status != UMI_STATUS_OK) return copy_status;
    return copy_text(event->message, sizeof(event->message), message);
}
