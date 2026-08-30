/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/content_runtime.h
 *
 * PURPOSE:
 *   Bind reusable Framework component hosts to federated desktop windows.
 *   Window content, application discovery, context sharing and cross-product
 *   messages remain mediated by Framework rather than direct application links.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_CONTENT_RUNTIME_H
#define UMICOM_DESKTOP_CONTENT_RUNTIME_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/application/federation.h"
#include "umicom/desktop/runtime.h"
#include "umicom/ui/components/host.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_CONTENT_MAX UMI_DESKTOP_MAX_WINDOWS

typedef struct UmiDesktopContentBinding {
    uint32_t structure_size;
    char window_id[UMI_DESKTOP_ID_CAPACITY];
    char view_type[UMI_UI_ID_CAPACITY];
    char view_id[UMI_UI_ID_CAPACITY];
    UmiUiWindowGroupRole context_role;
} UmiDesktopContentBinding;

typedef struct UmiDesktopContentMountRequest {
    uint32_t structure_size;
    UmiDesktopWindow window;
    char view_type[UMI_UI_ID_CAPACITY];
    char view_id[UMI_UI_ID_CAPACITY];
    UmiUiWindowGroupRole context_role;
} UmiDesktopContentMountRequest;

typedef struct UmiDesktopContentMessage {
    uint32_t structure_size;
    const char *message_id;
    const char *operation_id;
    const char *schema_id;
    const char *correlation_id;
    UmiFederationMessageKind kind;
    const void *payload;
    size_t payload_size;
} UmiDesktopContentMessage;

typedef struct UmiDesktopContentSnapshot {
    UmiDesktopWindow window;
    UmiUiComponentHostSnapshot host;
    char capability_id[UMI_DESKTOP_ID_CAPACITY];
    int owns_window;
    int joined_context;
    uint64_t revision;
} UmiDesktopContentSnapshot;

typedef struct UmiDesktopContentRuntime UmiDesktopContentRuntime;

UmiStatus umi_desktop_content_runtime_create(
    UmiDesktopRuntime *desktop,
    UmiFederationRouter *federation,
    UmiDesktopContentRuntime **out_runtime);
void umi_desktop_content_runtime_destroy(
    UmiDesktopContentRuntime *runtime);
UmiUiComponentHostService *umi_desktop_content_runtime_component_host(
    UmiDesktopContentRuntime *runtime);
UmiUiViewFactoryRegistry *umi_desktop_content_runtime_view_factories(
    UmiDesktopContentRuntime *runtime);
UmiStatus umi_desktop_content_runtime_bind_window(
    UmiDesktopContentRuntime *runtime,
    const UmiDesktopContentBinding *binding);
UmiStatus umi_desktop_content_runtime_mount(
    UmiDesktopContentRuntime *runtime,
    const UmiDesktopContentMountRequest *request);
UmiStatus umi_desktop_content_runtime_unmount(
    UmiDesktopContentRuntime *runtime,
    const char *window_id);
UmiStatus umi_desktop_content_runtime_activate(
    UmiDesktopContentRuntime *runtime,
    const char *window_id);
UmiStatus umi_desktop_content_runtime_suspend(
    UmiDesktopContentRuntime *runtime,
    const char *window_id);
UmiStatus umi_desktop_content_runtime_resume(
    UmiDesktopContentRuntime *runtime,
    const char *window_id);
UmiStatus umi_desktop_content_runtime_show(
    UmiDesktopContentRuntime *runtime,
    const char *window_id,
    int visible);
UmiStatus umi_desktop_content_runtime_refresh(
    UmiDesktopContentRuntime *runtime,
    const char *window_id);
UmiStatus umi_desktop_content_runtime_dispatch(
    UmiDesktopContentRuntime *runtime,
    const char *window_id,
    const UmiDesktopContentMessage *message,
    UmiFederationResponse *response);
UmiStatus umi_desktop_content_runtime_publish_context(
    UmiDesktopContentRuntime *runtime,
    const char *window_id,
    const char *context_type,
    const char *subject_id,
    const char *correlation_id);
UmiStatus umi_desktop_content_runtime_link_context(
    UmiDesktopContentRuntime *runtime,
    const char *window_id,
    const char *group_id,
    UmiUiWindowGroupRole role);
UmiStatus umi_desktop_content_runtime_snapshot(
    const UmiDesktopContentRuntime *runtime,
    const char *window_id,
    UmiDesktopContentSnapshot *out_snapshot);
UmiStatus umi_desktop_content_runtime_at(
    const UmiDesktopContentRuntime *runtime,
    size_t index,
    UmiDesktopContentSnapshot *out_snapshot);
UmiStatus umi_desktop_content_runtime_presentation(
    const UmiDesktopContentRuntime *runtime,
    const char *window_id,
    UmiUiViewPresentation *out_presentation);
size_t umi_desktop_content_runtime_count(
    const UmiDesktopContentRuntime *runtime);

#ifdef __cplusplus
}
#endif

#endif
