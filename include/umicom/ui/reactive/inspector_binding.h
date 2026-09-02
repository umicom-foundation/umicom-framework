/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/inspector_binding.h
 *
 * PURPOSE:
 *   Describe inspector subject and editing binding paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_INSPECTOR_BINDING_H
#define UMICOM_UI_REACTIVE_INSPECTOR_BINDING_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive inspector binding data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveInspectorBinding {
    char inspector_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char subject_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char edit_path[UMI_UI_REACTIVE_PATH_CAPACITY];
} UmiUiReactiveInspectorBinding;
/**
 * Initialise ui reactive inspector binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_inspector_binding_init(UmiUiReactiveInspectorBinding *item);
/**
 * Check that ui reactive inspector binding satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_inspector_binding_valid(const UmiUiReactiveInspectorBinding *item);
#ifdef __cplusplus
}
#endif
#endif
