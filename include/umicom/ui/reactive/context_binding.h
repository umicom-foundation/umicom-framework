/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/context_binding.h
 *
 * PURPOSE:
 *   Synchronise typed context channels with declarative UI properties.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CONTEXT_BINDING_H
#define UMICOM_UI_REACTIVE_CONTEXT_BINDING_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive context binding data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveContextBinding {
    char channel_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char property_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    bool publish;
    bool subscribe;
} UmiUiReactiveContextBinding;
/**
 * Initialise ui reactive context binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_context_binding_init(UmiUiReactiveContextBinding *item);
/**
 * Check that ui reactive context binding satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_context_binding_valid(const UmiUiReactiveContextBinding *item);
#ifdef __cplusplus
}
#endif
#endif
