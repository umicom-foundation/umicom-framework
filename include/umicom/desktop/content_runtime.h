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

/**
 * Represent the desktop content binding data shared with callers of this public contract.
 */
typedef struct UmiDesktopContentBinding {
    uint32_t structure_size;
    char window_id[UMI_DESKTOP_ID_CAPACITY];
    char view_type[UMI_UI_ID_CAPACITY];
    char view_id[UMI_UI_ID_CAPACITY];
    UmiUiWindowGroupRole context_role;
} UmiDesktopContentBinding;

/**
 * Represent the desktop content mount request data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopContentMountRequest {
    uint32_t structure_size;
    UmiDesktopWindow window;
    char view_type[UMI_UI_ID_CAPACITY];
    char view_id[UMI_UI_ID_CAPACITY];
    UmiUiWindowGroupRole context_role;
} UmiDesktopContentMountRequest;

/**
 * Represent the desktop content message data shared with callers of this public contract.
 */
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

/**
 * Represent the desktop content snapshot data shared with callers of this public contract.
 */
typedef struct UmiDesktopContentSnapshot {
    UmiDesktopWindow window;
    UmiUiComponentHostSnapshot host;
    char capability_id[UMI_DESKTOP_ID_CAPACITY];
    int owns_window;
    int joined_context;
    uint64_t revision;
} UmiDesktopContentSnapshot;

/**
 * Represent the desktop content runtime data shared with callers of this public contract.
 */
typedef struct UmiDesktopContentRuntime UmiDesktopContentRuntime;

/**
 * Initialise desktop content runtime from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_desktop_content_runtime_create(
    UmiDesktopRuntime *desktop,
    UmiFederationRouter *federation,
    UmiDesktopContentRuntime **out_runtime);
/**
 * Release or reset state held by desktop content runtime so the same storage can be reused
 * safely.
 */
void umi_desktop_content_runtime_destroy(
    UmiDesktopContentRuntime *runtime);
/**
 * Provide the desktop content runtime component host operation used by this module and its
 * client applications.
 */
UmiUiComponentHostService *umi_desktop_content_runtime_component_host(
    UmiDesktopContentRuntime *runtime);
/**
 * Provide the desktop content runtime view factories operation used by this module and its
 * client applications.
 */
UmiUiViewFactoryRegistry *umi_desktop_content_runtime_view_factories(
    UmiDesktopContentRuntime *runtime);
/**
 * Provide the desktop content runtime bind window operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_content_runtime_bind_window(
    UmiDesktopContentRuntime *runtime,
    const UmiDesktopContentBinding *binding);
/**
 * Provide the desktop content runtime mount operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_content_runtime_mount(
    UmiDesktopContentRuntime *runtime,
    const UmiDesktopContentMountRequest *request);
/**
 * Provide the desktop content runtime unmount operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_content_runtime_unmount(
    UmiDesktopContentRuntime *runtime,
    const char *window_id);
/**
 * Provide the desktop content runtime activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_content_runtime_activate(
    UmiDesktopContentRuntime *runtime,
    const char *window_id);
/**
 * Provide the desktop content runtime suspend operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_content_runtime_suspend(
    UmiDesktopContentRuntime *runtime,
    const char *window_id);
/**
 * Provide the desktop content runtime resume operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_content_runtime_resume(
    UmiDesktopContentRuntime *runtime,
    const char *window_id);
/**
 * Provide the desktop content runtime show operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_content_runtime_show(
    UmiDesktopContentRuntime *runtime,
    const char *window_id,
    int visible);
/**
 * Provide the desktop content runtime refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_content_runtime_refresh(
    UmiDesktopContentRuntime *runtime,
    const char *window_id);
/**
 * Perform desktop content runtime through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_desktop_content_runtime_dispatch(
    UmiDesktopContentRuntime *runtime,
    const char *window_id,
    const UmiDesktopContentMessage *message,
    UmiFederationResponse *response);
/**
 * Provide the desktop content runtime publish context operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_content_runtime_publish_context(
    UmiDesktopContentRuntime *runtime,
    const char *window_id,
    const char *context_type,
    const char *subject_id,
    const char *correlation_id);
/**
 * Provide the desktop content runtime link context operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_content_runtime_link_context(
    UmiDesktopContentRuntime *runtime,
    const char *window_id,
    const char *group_id,
    UmiUiWindowGroupRole role);
/**
 * Provide the desktop content runtime snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_content_runtime_snapshot(
    const UmiDesktopContentRuntime *runtime,
    const char *window_id,
    UmiDesktopContentSnapshot *out_snapshot);
/**
 * Find desktop content runtime while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_desktop_content_runtime_at(
    const UmiDesktopContentRuntime *runtime,
    size_t index,
    UmiDesktopContentSnapshot *out_snapshot);
/**
 * Provide the desktop content runtime presentation operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_content_runtime_presentation(
    const UmiDesktopContentRuntime *runtime,
    const char *window_id,
    UmiUiViewPresentation *out_presentation);
/**
 * Return the number of records represented by desktop content runtime without changing
 * their state.
 */
size_t umi_desktop_content_runtime_count(
    const UmiDesktopContentRuntime *runtime);

#ifdef __cplusplus
}
#endif

#endif
