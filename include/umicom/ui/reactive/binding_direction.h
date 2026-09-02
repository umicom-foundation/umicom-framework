/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/binding_direction.h
 *
 * PURPOSE:
 *   Describe binding direction and update trigger policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BINDING_DIRECTION_H
#define UMICOM_UI_REACTIVE_BINDING_DIRECTION_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive binding direction policy data shared with callers of this
 * public contract.
 */
typedef struct UmiUiReactiveBindingDirectionPolicy {
    UmiUiReactiveBindingDirection direction;
    UmiUiReactiveUpdateTrigger trigger;
    bool propagate_initial;
} UmiUiReactiveBindingDirectionPolicy;
/**
 * Initialise ui reactive binding direction from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_binding_direction_init(UmiUiReactiveBindingDirectionPolicy *item);
/**
 * Check that ui reactive binding direction satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_binding_direction_valid(const UmiUiReactiveBindingDirectionPolicy *item);
#ifdef __cplusplus
}
#endif
#endif
