/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/tab_stack.c
 *
 * PURPOSE:
 *   Implement a deterministic bounded tab stack for editors, tools, charts and other workstation surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/tab_stack.h"

UmiStatus umi_ws_tab_stack_init(UmiWsTabStack *stack, const char *stack_id) {
    if (stack == NULL || !umi_ws_id_valid(stack_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *stack = (UmiWsTabStack){0};
    stack->allow_reorder = true;
    return umi_ws_copy_text(stack->stack_id, sizeof(stack->stack_id), stack_id);
}

UmiStatus umi_ws_tab_stack_add(UmiWsTabStack *stack, const char *surface_id) {
    size_t index;
    if (stack == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < stack->count; ++index) {
        if (strcmp(stack->tabs[index], surface_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    }
    if (stack->count >= UMI_WS_MAX_TABS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(stack->tabs[stack->count], sizeof(stack->tabs[stack->count]), surface_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++stack->count;
    if (stack->count == 1U) stack->active_index = 0U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_tab_stack_remove(UmiWsTabStack *stack, const char *surface_id) {
    size_t index;
    if (stack == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < stack->count; ++index) {
        if (strcmp(stack->tabs[index], surface_id) == 0) {
            size_t move;
            for (move = index; move + 1U < stack->count; ++move) {
                memcpy(stack->tabs[move], stack->tabs[move + 1U], UMI_UI_ID_CAPACITY);
            }
            --stack->count;
            if (stack->count == 0U) stack->active_index = 0U;
            else if (stack->active_index >= stack->count) stack->active_index = stack->count - 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_ws_tab_stack_activate(UmiWsTabStack *stack, const char *surface_id) {
    size_t index;
    if (stack == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < stack->count; ++index) {
        if (strcmp(stack->tabs[index], surface_id) == 0) {
            stack->active_index = index;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

const char *umi_ws_tab_stack_active(const UmiWsTabStack *stack) {
    if (stack == NULL || stack->count == 0U || stack->active_index >= stack->count) return NULL;
    return stack->tabs[stack->active_index];
}
