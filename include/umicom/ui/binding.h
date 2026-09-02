/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/binding.h
 *
 * PURPOSE:
 *   Define property bindings between view models with explicit one-way direction,
 *   optional transforms and deterministic propagation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * Represent the ui binding descriptor data shared with callers of this public contract.
 */
typedef struct UmiUiBindingDescriptor { char binding_id[UMI_UI_ID_CAPACITY]; UmiUiViewModel *source_view; char source_property[UMI_UI_PROPERTY_KEY_CAPACITY]; UmiUiViewModel *target_view; char target_property[UMI_UI_PROPERTY_KEY_CAPACITY]; UmiUiBindingTransform transform; void *user_data; int enabled; } UmiUiBindingDescriptor;
/**
 * Represent the ui binding registry data shared with callers of this public contract.
 */
typedef struct UmiUiBindingRegistry UmiUiBindingRegistry;
/**
 * Initialise ui binding registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_binding_registry_create(UmiUiBindingRegistry **out_registry);
/**
 * Release or reset state held by ui binding registry so the same storage can be reused
 * safely.
 */
void umi_ui_binding_registry_destroy(UmiUiBindingRegistry *registry);
/**
 * Add ui binding only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ui_binding_register(UmiUiBindingRegistry *registry, const UmiUiBindingDescriptor *descriptor);
/**
 * Remove ui binding while keeping the remaining records in a valid and discoverable state.
 */
UmiStatus umi_ui_binding_unregister(UmiUiBindingRegistry *registry, const char *binding_id);
/**
 * Provide the ui binding propagate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_binding_propagate(UmiUiBindingRegistry *registry, const char *binding_id);
/**
 * Provide the ui binding propagate all operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_binding_propagate_all(UmiUiBindingRegistry *registry, size_t *out_propagated);
/**
 * Return the number of records represented by ui binding without changing their state.
 */
size_t umi_ui_binding_count(const UmiUiBindingRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
