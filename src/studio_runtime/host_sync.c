/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/host_sync.c
 *
 * PURPOSE:
 *   Implement Framework-to-toolkit presentation synchronization.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/host_sync.h"

UmiStatus umi_studio_host_sync(
    UmiStudioRuntimePlatform *platform,
    const UmiStudioRuntimeHostAdapter *adapter)
{
    UmiStudioRuntimeSnapshot snapshot;
    UmiStudioRuntimeBindings *bindings;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_studio_host_adapter_validate(adapter);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_runtime_platform_snapshot(platform, &snapshot);
    if (status != UMI_STATUS_OK) return status;

    bindings = umi_studio_runtime_platform_bindings(platform);
    if (bindings == NULL) return UMI_STATUS_INVALID_STATE;

    status = adapter->set_window_title(
        adapter->user_data,
        snapshot.window_title);
    if (status == UMI_STATUS_OK) {
        status = adapter->present_surface(
            adapter->user_data,
            bindings->shell_state);
    }
    if (status == UMI_STATUS_OK) {
        status = adapter->update_status(
            adapter->user_data,
            &snapshot.status);
    }
    if (status == UMI_STATUS_OK) {
        status = adapter->update_document_tabs(
            adapter->user_data,
            &snapshot.tabs);
    }
    if (status == UMI_STATUS_OK) {
        status = adapter->update_workflow(
            adapter->user_data,
            &snapshot.ide.workflow);
    }

    return status;
}
