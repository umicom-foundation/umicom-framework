/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/grid_binding.h
 *
 * PURPOSE:
 *   Describe enterprise grid provider/query/selection binding paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_GRID_BINDING_H
#define UMICOM_UI_REACTIVE_GRID_BINDING_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive grid binding data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveGridBinding {
    char grid_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char provider_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char query_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char selection_path[UMI_UI_REACTIVE_PATH_CAPACITY];
} UmiUiReactiveGridBinding;
/**
 * Initialise ui reactive grid binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_grid_binding_init(UmiUiReactiveGridBinding *item);
/**
 * Check that ui reactive grid binding satisfies its contract before another service relies
 * on it.
 */
int umi_ui_reactive_grid_binding_valid(const UmiUiReactiveGridBinding *item);
#ifdef __cplusplus
}
#endif
#endif
