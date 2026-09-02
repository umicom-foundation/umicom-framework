/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/tab_stack.h
 *
 * PURPOSE:
 *   Define a deterministic bounded tab stack for editors, tools, charts and other workstation surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_TAB_STACK_H
#define UMICOM_UI_WORKSTATION_TAB_STACK_H

#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws tab stack data shared with callers of this public contract.
 */
typedef struct UmiWsTabStack {
    char stack_id[UMI_UI_ID_CAPACITY];
    char tabs[UMI_WS_MAX_TABS][UMI_UI_ID_CAPACITY];
    size_t count;
    size_t active_index;
    bool allow_reorder;
} UmiWsTabStack;

/**
 * Initialise ws tab stack from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ws_tab_stack_init(UmiWsTabStack *stack, const char *stack_id);
/**
 * Add ws tab stack only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ws_tab_stack_add(UmiWsTabStack *stack, const char *surface_id);
/**
 * Remove ws tab stack while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ws_tab_stack_remove(UmiWsTabStack *stack, const char *surface_id);
/**
 * Provide the ws tab stack activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_tab_stack_activate(UmiWsTabStack *stack, const char *surface_id);
/**
 * Provide the ws tab stack active operation used by this module and its client
 * applications.
 */
const char *umi_ws_tab_stack_active(const UmiWsTabStack *stack);

#ifdef __cplusplus
}
#endif

#endif
