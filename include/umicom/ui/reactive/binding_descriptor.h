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
typedef struct UmiUiReactiveBindingDescriptor {
    char binding_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char source_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char target_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    UmiUiReactiveBindingDirection direction;
    UmiUiReactiveUpdateTrigger trigger;
    bool enabled;
} UmiUiReactiveBindingDescriptor;
void umi_ui_reactive_binding_descriptor_init(UmiUiReactiveBindingDescriptor *item);
int umi_ui_reactive_binding_descriptor_valid(const UmiUiReactiveBindingDescriptor *item);
#ifdef __cplusplus
}
#endif
#endif
