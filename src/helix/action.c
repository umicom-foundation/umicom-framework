/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/action.c
 * PURPOSE: Initialise bounded Helix v2 action descriptors.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/action.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

/*
 * Initialise helix action from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_helix_action_init(UmiHelixAction *action,
                                   const char *action_id,
                                   UmiHelixActionKind kind,
                                   const char *summary,
                                   const char *target,
                                   uint32_t maximum_attempts)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (action == NULL || action_id == NULL || summary == NULL ||
        target == NULL || maximum_attempts == 0U ||
        kind < UMI_HELIX_ACTION_READ || kind > UMI_HELIX_ACTION_SOURCE_CONTROL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(action, 0, sizeof(*action));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!copy_text(action->action_id, sizeof(action->action_id), action_id) ||
        !copy_text(action->summary, sizeof(action->summary), summary) ||
        !copy_text(action->target, sizeof(action->target), target)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    action->kind = kind;
    action->maximum_attempts = maximum_attempts;
    action->mutating = umi_helix_action_kind_is_mutating(kind);
    return UMI_STATUS_OK;
}

/*
 * Provide the helix action requires approval operation used by this module and its client
 * applications.
 */
int umi_helix_action_requires_approval(const UmiHelixAction *action)
{
    return action != NULL && action->mutating;
}
