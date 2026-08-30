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
typedef struct UmiUiReactiveBindingDirectionPolicy {
    UmiUiReactiveBindingDirection direction;
    UmiUiReactiveUpdateTrigger trigger;
    bool propagate_initial;
} UmiUiReactiveBindingDirectionPolicy;
void umi_ui_reactive_binding_direction_init(UmiUiReactiveBindingDirectionPolicy *item);
int umi_ui_reactive_binding_direction_valid(const UmiUiReactiveBindingDirectionPolicy *item);
#ifdef __cplusplus
}
#endif
#endif
