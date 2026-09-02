/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/host.h
 *
 * PURPOSE:
 *   Describe a reusable hosted-content slot for browser, media, editor or chart adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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
/**
 * Represent the ui component host data shared with callers of this public contract.
 */
typedef struct UmiUiComponentHost { char kind[UMI_UI_HOST_KIND_CAPACITY]; void *adapter_handle; } UmiUiComponentHost;
/**
 * Initialise ui component host from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_component_host_init(UmiUiComponentHost *host,const char *kind,void *adapter_handle);

#define UMI_UI_COMPONENT_HOST_MAX 128U

/**
 * List the named ui component host state values accepted by this public contract.
 */
typedef enum UmiUiComponentHostState {
    UMI_UI_COMPONENT_HOST_MOUNTED = 1,
    UMI_UI_COMPONENT_HOST_ACTIVE = 2,
    UMI_UI_COMPONENT_HOST_SUSPENDED = 3,
    UMI_UI_COMPONENT_HOST_FAILED = 4
} UmiUiComponentHostState;

/**
 * Represent the ui component host descriptor data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the ui component host snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiComponentHostSnapshot {
    UmiUiComponentHostDescriptor descriptor;
    UmiUiComponentHostState state;
    size_t presentation_property_count;
    uint64_t revision;
} UmiUiComponentHostSnapshot;

/**
 * Represent the ui component host service data shared with callers of this public
 * contract.
 */
typedef struct UmiUiComponentHostService UmiUiComponentHostService;

/**
 * Initialise ui component host service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_component_host_service_create(
    UmiUiComponentHostService **out_service);
/**
 * Release or reset state held by ui component host service so the same storage can be
 * reused safely.
 */
void umi_ui_component_host_service_destroy(
    UmiUiComponentHostService *service);
/**
 * Provide the ui component host view factories operation used by this module and its
 * client applications.
 */
UmiUiViewFactoryRegistry *umi_ui_component_host_view_factories(
    UmiUiComponentHostService *service);
/**
 * Provide the ui component host mount operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_host_mount(
    UmiUiComponentHostService *service,
    const UmiUiComponentHostDescriptor *descriptor);
/**
 * Provide the ui component host unmount operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_host_unmount(
    UmiUiComponentHostService *service,
    const char *host_id);
/**
 * Provide the ui component host activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_host_activate(
    UmiUiComponentHostService *service,
    const char *host_id);
/**
 * Provide the ui component host suspend operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_host_suspend(
    UmiUiComponentHostService *service,
    const char *host_id);
/**
 * Provide the ui component host resume operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_host_resume(
    UmiUiComponentHostService *service,
    const char *host_id);
/**
 * Provide the ui component host show operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_host_show(
    UmiUiComponentHostService *service,
    const char *host_id,
    int visible);
/**
 * Provide the ui component host refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_host_refresh(
    UmiUiComponentHostService *service,
    const char *host_id);
/**
 * Provide the ui component host snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_host_snapshot(
    const UmiUiComponentHostService *service,
    const char *host_id,
    UmiUiComponentHostSnapshot *out_snapshot);
/**
 * Find ui component host while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_component_host_at(
    const UmiUiComponentHostService *service,
    size_t index,
    UmiUiComponentHostSnapshot *out_snapshot);
/**
 * Provide the ui component host presentation operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_component_host_presentation(
    const UmiUiComponentHostService *service,
    const char *host_id,
    UmiUiViewPresentation *out_presentation);
/**
 * Return the number of records represented by ui component host without changing their
 * state.
 */
size_t umi_ui_component_host_count(
    const UmiUiComponentHostService *service);

#ifdef __cplusplus
}
#endif

#endif
