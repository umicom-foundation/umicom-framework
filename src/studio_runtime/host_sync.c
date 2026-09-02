/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/host_sync.c
 *
 * PURPOSE:
 *   Implement Framework-to-toolkit presentation synchronization.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/host_sync.h"

/* Provide the studio host sync operation used by this module and its client applications. */
UmiStatus umi_studio_host_sync(
    UmiStudioRuntimePlatform *platform,
    const UmiStudioRuntimeHostAdapter *adapter)
{
    UmiStudioRuntimeSnapshot snapshot;
    UmiStudioRuntimeBindings *bindings;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_studio_host_adapter_validate(adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_runtime_platform_snapshot(platform, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    bindings = umi_studio_runtime_platform_bindings(platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL) return UMI_STATUS_INVALID_STATE;

    status = adapter->set_window_title(
        adapter->user_data,
        snapshot.window_title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = adapter->present_surface(
            adapter->user_data,
            bindings->shell_state);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = adapter->update_status(
            adapter->user_data,
            &snapshot.status);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = adapter->update_document_tabs(
            adapter->user_data,
            &snapshot.tabs);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = adapter->update_workflow(
            adapter->user_data,
            &snapshot.ide.workflow);
    }

    return status;
}
