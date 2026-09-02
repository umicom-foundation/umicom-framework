/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/tab_stack.c
 *
 * PURPOSE:
 *   Implement a deterministic bounded tab stack for editors, tools, charts and other workstation surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/tab_stack.h"

/*
 * Initialise ws tab stack from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ws_tab_stack_init(UmiWsTabStack *stack, const char *stack_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stack == NULL || !umi_ws_id_valid(stack_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *stack = (UmiWsTabStack){0};
    stack->allow_reorder = true;
    return umi_ws_copy_text(stack->stack_id, sizeof(stack->stack_id), stack_id);
}

/* Add ws tab stack only after its inputs and available capacity have been checked. */
UmiStatus umi_ws_tab_stack_add(UmiWsTabStack *stack, const char *surface_id) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stack == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < stack->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(stack->tabs[index], surface_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (stack->count >= UMI_WS_MAX_TABS) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_ws_copy_text(stack->tabs[stack->count], sizeof(stack->tabs[stack->count]), surface_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++stack->count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (stack->count == 1U) stack->active_index = 0U;
    return UMI_STATUS_OK;
}

/*
 * Remove ws tab stack while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ws_tab_stack_remove(UmiWsTabStack *stack, const char *surface_id) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stack == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < stack->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(stack->tabs[index], surface_id) == 0) {
            size_t move;
            /* Visit each bounded item once so every record receives the same rule. */
            for (move = index; move + 1U < stack->count; ++move) {
                memcpy(stack->tabs[move], stack->tabs[move + 1U], UMI_UI_ID_CAPACITY);
            }
            --stack->count;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (stack->count == 0U) stack->active_index = 0U;
            else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (stack->active_index >= stack->count) stack->active_index = stack->count - 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the ws tab stack activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_tab_stack_activate(UmiWsTabStack *stack, const char *surface_id) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stack == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < stack->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(stack->tabs[index], surface_id) == 0) {
            stack->active_index = index;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the ws tab stack active operation used by this module and its client
 * applications.
 */
const char *umi_ws_tab_stack_active(const UmiWsTabStack *stack) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stack == NULL || stack->count == 0U || stack->active_index >= stack->count) return NULL;
    return stack->tabs[stack->active_index];
}
