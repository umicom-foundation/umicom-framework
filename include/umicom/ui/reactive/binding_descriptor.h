/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/binding_descriptor.h
 *
 * PURPOSE:
 *   Describe a declarative binding between source and target endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BINDING_DESCRIPTOR_H
#define UMICOM_UI_REACTIVE_BINDING_DESCRIPTOR_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive binding descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveBindingDescriptor {
    char binding_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char source_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char target_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    UmiUiReactiveBindingDirection direction;
    UmiUiReactiveUpdateTrigger trigger;
    bool enabled;
} UmiUiReactiveBindingDescriptor;
/**
 * Initialise ui reactive binding descriptor from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_binding_descriptor_init(UmiUiReactiveBindingDescriptor *item);
/**
 * Check that ui reactive binding descriptor satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_binding_descriptor_valid(const UmiUiReactiveBindingDescriptor *item);
#ifdef __cplusplus
}
#endif
#endif
