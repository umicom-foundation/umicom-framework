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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/event.h"

#include <string.h>

static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;
    if (out == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_event_init(
    UmiRepositoryControlEvent *event,
    uint64_t sequence,
    UmiRepositoryControlState state,
    UmiStatus status,
    const char *path,
    const char *message)
{
    UmiStatus copy_status;
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
    if (copy_status != UMI_STATUS_OK) return copy_status;
    return copy_text(event->message, sizeof(event->message), message);
}
