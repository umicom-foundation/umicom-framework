/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/operation.c
 * PURPOSE: Enforce deterministic Helix v2 operation transitions.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/operation.h"

#include <stdio.h>
#include <string.h>

static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

UmiStatus umi_helix_operation_init(UmiHelixOperation *operation,
                                      const char *operation_id,
                                      const char *objective,
                                      const char *plan_hash)
{
    if (operation == NULL || operation_id == NULL || objective == NULL ||
        plan_hash == NULL || operation_id[0] == '\0' || plan_hash[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(operation, 0, sizeof(*operation));
    if (!copy_text(operation->operation_id, sizeof(operation->operation_id),
                   operation_id) ||
        !copy_text(operation->objective, sizeof(operation->objective), objective) ||
        !copy_text(operation->plan_hash, sizeof(operation->plan_hash), plan_hash)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    operation->state = UMI_HELIX_OPERATION_DRAFT;
    operation->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_helix_operation_transition(
    UmiHelixOperation *operation,
    UmiHelixOperationState next)
{
    if (operation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_helix_operation_state_can_transition(operation->state, next)) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (operation->state != next) {
        operation->state = next;
        operation->revision++;
    }
    return UMI_STATUS_OK;
}
