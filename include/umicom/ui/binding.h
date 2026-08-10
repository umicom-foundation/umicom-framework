/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/binding.h
 *
 * PURPOSE:
 *   Define property bindings between view models with explicit one-way direction,
 *   optional transforms and deterministic propagation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_BINDING_H
#define UMICOM_UI_BINDING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_BINDING_MAX 256U
typedef UmiStatus (*UmiUiBindingTransform)(const UmiUiValue *source, UmiUiValue *target, void *user_data);
typedef struct UmiUiBindingDescriptor { char binding_id[UMI_UI_ID_CAPACITY]; UmiUiViewModel *source_view; char source_property[UMI_UI_PROPERTY_KEY_CAPACITY]; UmiUiViewModel *target_view; char target_property[UMI_UI_PROPERTY_KEY_CAPACITY]; UmiUiBindingTransform transform; void *user_data; int enabled; } UmiUiBindingDescriptor;
typedef struct UmiUiBindingRegistry UmiUiBindingRegistry;
UmiStatus umi_ui_binding_registry_create(UmiUiBindingRegistry **out_registry);
void umi_ui_binding_registry_destroy(UmiUiBindingRegistry *registry);
UmiStatus umi_ui_binding_register(UmiUiBindingRegistry *registry, const UmiUiBindingDescriptor *descriptor);
UmiStatus umi_ui_binding_unregister(UmiUiBindingRegistry *registry, const char *binding_id);
UmiStatus umi_ui_binding_propagate(UmiUiBindingRegistry *registry, const char *binding_id);
UmiStatus umi_ui_binding_propagate_all(UmiUiBindingRegistry *registry, size_t *out_propagated);
size_t umi_ui_binding_count(const UmiUiBindingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
