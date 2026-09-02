/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/selection_binding.h
 *
 * PURPOSE:
 *   Synchronise canonical selection context with a semantic surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_SELECTION_BINDING_H
#define UMICOM_UI_REACTIVE_SELECTION_BINDING_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive selection binding data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveSelectionBinding {
    char surface_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char selection_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    bool two_way;
} UmiUiReactiveSelectionBinding;
/**
 * Initialise ui reactive selection binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_selection_binding_init(UmiUiReactiveSelectionBinding *item);
/**
 * Check that ui reactive selection binding satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_selection_binding_valid(const UmiUiReactiveSelectionBinding *item);
#ifdef __cplusplus
}
#endif
#endif
