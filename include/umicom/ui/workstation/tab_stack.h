/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/tab_stack.h
 *
 * PURPOSE:
 *   Define a deterministic bounded tab stack for editors, tools, charts and other workstation surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_TAB_STACK_H
#define UMICOM_UI_WORKSTATION_TAB_STACK_H

#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsTabStack {
    char stack_id[UMI_UI_ID_CAPACITY];
    char tabs[UMI_WS_MAX_TABS][UMI_UI_ID_CAPACITY];
    size_t count;
    size_t active_index;
    bool allow_reorder;
} UmiWsTabStack;

UmiStatus umi_ws_tab_stack_init(UmiWsTabStack *stack, const char *stack_id);
UmiStatus umi_ws_tab_stack_add(UmiWsTabStack *stack, const char *surface_id);
UmiStatus umi_ws_tab_stack_remove(UmiWsTabStack *stack, const char *surface_id);
UmiStatus umi_ws_tab_stack_activate(UmiWsTabStack *stack, const char *surface_id);
const char *umi_ws_tab_stack_active(const UmiWsTabStack *stack);

#ifdef __cplusplus
}
#endif

#endif
