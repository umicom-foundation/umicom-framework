/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/host.h
 *
 * PURPOSE:
 *   Describe a reusable hosted-content slot for browser, media, editor or chart adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_HOST_H
#define UMICOM_UI_COMPONENTS_HOST_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ui/view_presentation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_HOST_KIND_CAPACITY 64U
typedef struct UmiUiComponentHost { char kind[UMI_UI_HOST_KIND_CAPACITY]; void *adapter_handle; } UmiUiComponentHost;
UmiStatus umi_ui_component_host_init(UmiUiComponentHost *host,const char *kind,void *adapter_handle);

#define UMI_UI_COMPONENT_HOST_MAX 128U

typedef enum UmiUiComponentHostState {
    UMI_UI_COMPONENT_HOST_MOUNTED = 1,
    UMI_UI_COMPONENT_HOST_ACTIVE = 2,
    UMI_UI_COMPONENT_HOST_SUSPENDED = 3,
    UMI_UI_COMPONENT_HOST_FAILED = 4
} UmiUiComponentHostState;

typedef struct UmiUiComponentHostDescriptor {
    uint32_t structure_size;
    char host_id[UMI_UI_ID_CAPACITY];
    char component_id[UMI_UI_ID_CAPACITY];
    char owner_application_id[UMI_UI_ID_CAPACITY];
    char view_type[UMI_UI_ID_CAPACITY];
    char view_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    int visible;
} UmiUiComponentHostDescriptor;

typedef struct UmiUiComponentHostSnapshot {
    UmiUiComponentHostDescriptor descriptor;
    UmiUiComponentHostState state;
    size_t presentation_property_count;
    uint64_t revision;
} UmiUiComponentHostSnapshot;

typedef struct UmiUiComponentHostService UmiUiComponentHostService;

UmiStatus umi_ui_component_host_service_create(
    UmiUiComponentHostService **out_service);
void umi_ui_component_host_service_destroy(
    UmiUiComponentHostService *service);
UmiUiViewFactoryRegistry *umi_ui_component_host_view_factories(
    UmiUiComponentHostService *service);
UmiStatus umi_ui_component_host_mount(
    UmiUiComponentHostService *service,
    const UmiUiComponentHostDescriptor *descriptor);
UmiStatus umi_ui_component_host_unmount(
    UmiUiComponentHostService *service,
    const char *host_id);
UmiStatus umi_ui_component_host_activate(
    UmiUiComponentHostService *service,
    const char *host_id);
UmiStatus umi_ui_component_host_suspend(
    UmiUiComponentHostService *service,
    const char *host_id);
UmiStatus umi_ui_component_host_resume(
    UmiUiComponentHostService *service,
    const char *host_id);
UmiStatus umi_ui_component_host_show(
    UmiUiComponentHostService *service,
    const char *host_id,
    int visible);
UmiStatus umi_ui_component_host_refresh(
    UmiUiComponentHostService *service,
    const char *host_id);
UmiStatus umi_ui_component_host_snapshot(
    const UmiUiComponentHostService *service,
    const char *host_id,
    UmiUiComponentHostSnapshot *out_snapshot);
UmiStatus umi_ui_component_host_at(
    const UmiUiComponentHostService *service,
    size_t index,
    UmiUiComponentHostSnapshot *out_snapshot);
UmiStatus umi_ui_component_host_presentation(
    const UmiUiComponentHostService *service,
    const char *host_id,
    UmiUiViewPresentation *out_presentation);
size_t umi_ui_component_host_count(
    const UmiUiComponentHostService *service);

#ifdef __cplusplus
}
#endif

#endif
