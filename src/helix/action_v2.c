/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/action_v2.c
 * PURPOSE: Initialise bounded Helix v2 action descriptors.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/action_v2.h"

#include <stdio.h>
#include <string.h>

static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

UmiStatus umi_helix_action_v2_init(UmiHelixActionV2 *action,
                                   const char *action_id,
                                   UmiHelixActionKindV2 kind,
                                   const char *summary,
                                   const char *target,
                                   uint32_t maximum_attempts)
{
    if (action == NULL || action_id == NULL || summary == NULL ||
        target == NULL || maximum_attempts == 0U ||
        kind < UMI_HELIX_ACTION_READ || kind > UMI_HELIX_ACTION_SOURCE_CONTROL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(action, 0, sizeof(*action));
    if (!copy_text(action->action_id, sizeof(action->action_id), action_id) ||
        !copy_text(action->summary, sizeof(action->summary), summary) ||
        !copy_text(action->target, sizeof(action->target), target)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    action->kind = kind;
    action->maximum_attempts = maximum_attempts;
    action->mutating = umi_helix_action_kind_v2_is_mutating(kind);
    return UMI_STATUS_OK;
}

int umi_helix_action_v2_requires_approval(const UmiHelixActionV2 *action)
{
    return action != NULL && action->mutating;
}
