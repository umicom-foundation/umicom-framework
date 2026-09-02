/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/command_binding.h
 *
 * PURPOSE:
 *   Connect a semantic command to reactive enablement and parameter state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_COMMAND_BINDING_H
#define UMICOM_UI_REACTIVE_COMMAND_BINDING_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive command binding data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveCommandBinding {
    char binding_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char command_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char parameter_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    bool enabled;
} UmiUiReactiveCommandBinding;
/**
 * Initialise ui reactive command binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_command_binding_init(UmiUiReactiveCommandBinding *item);
/**
 * Check that ui reactive command binding satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_command_binding_valid(const UmiUiReactiveCommandBinding *item);
#ifdef __cplusplus
}
#endif
#endif
