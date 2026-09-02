/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/view_factory.h
 *
 * PURPOSE:
 *   Define factories that allow modules and plug-ins to provide frontend-neutral view
 *   models without constructing toolkit widgets directly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_VIEW_FACTORY_H
#define UMICOM_UI_VIEW_FACTORY_H

#include <stddef.h>
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_VIEW_FACTORY_MAX 128U
typedef UmiStatus (*UmiUiViewCreateFn)(const char *view_id, void *user_data, UmiUiViewModel **out_view);
/**
 * Represent the ui view factory descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiUiViewFactoryDescriptor { char view_type[UMI_UI_ID_CAPACITY]; char provider_id[UMI_UI_ID_CAPACITY]; UmiUiViewCreateFn create; void *user_data; } UmiUiViewFactoryDescriptor;
/**
 * Represent the ui view factory registry data shared with callers of this public contract.
 */
typedef struct UmiUiViewFactoryRegistry UmiUiViewFactoryRegistry;
/**
 * Initialise ui view factory registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_view_factory_registry_create(UmiUiViewFactoryRegistry **out_registry);
/**
 * Release or reset state held by ui view factory registry so the same storage can be
 * reused safely.
 */
void umi_ui_view_factory_registry_destroy(UmiUiViewFactoryRegistry *registry);
/**
 * Add ui view factory only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ui_view_factory_register(UmiUiViewFactoryRegistry *registry, const UmiUiViewFactoryDescriptor *descriptor);
/**
 * Remove ui view factory while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_view_factory_unregister(UmiUiViewFactoryRegistry *registry, const char *view_type);
/**
 * Provide the ui view factory create view operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_factory_create_view(const UmiUiViewFactoryRegistry *registry, const char *view_type, const char *view_id, UmiUiViewModel **out_view);
/**
 * Return the number of records represented by ui view factory without changing their
 * state.
 */
size_t umi_ui_view_factory_count(const UmiUiViewFactoryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
