/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/view_factory.h
 *
 * PURPOSE:
 *   Define factories that allow modules and plug-ins to provide frontend-neutral view
 *   models without constructing toolkit widgets directly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiUiViewFactoryDescriptor { char view_type[UMI_UI_ID_CAPACITY]; char provider_id[UMI_UI_ID_CAPACITY]; UmiUiViewCreateFn create; void *user_data; } UmiUiViewFactoryDescriptor;
typedef struct UmiUiViewFactoryRegistry UmiUiViewFactoryRegistry;
UmiStatus umi_ui_view_factory_registry_create(UmiUiViewFactoryRegistry **out_registry);
void umi_ui_view_factory_registry_destroy(UmiUiViewFactoryRegistry *registry);
UmiStatus umi_ui_view_factory_register(UmiUiViewFactoryRegistry *registry, const UmiUiViewFactoryDescriptor *descriptor);
UmiStatus umi_ui_view_factory_unregister(UmiUiViewFactoryRegistry *registry, const char *view_type);
UmiStatus umi_ui_view_factory_create_view(const UmiUiViewFactoryRegistry *registry, const char *view_type, const char *view_id, UmiUiViewModel **out_view);
size_t umi_ui_view_factory_count(const UmiUiViewFactoryRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
