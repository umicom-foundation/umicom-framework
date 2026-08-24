/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/document_sync.c
 *
 * PURPOSE:
 *   Implement DocumentCoordinator -> editor focus/title synchronization.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/document_sync.h"

#include <string.h>

UmiStatus umi_studio_document_sync_init(
    UmiStudioRuntimeDocumentSync *sync,
    const char *workspace_name)
{
    size_t length;

    if (sync == NULL || workspace_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(workspace_name);
    if (length >= sizeof(sync->workspace_name)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(sync, 0, sizeof(*sync));
    (void)memcpy(sync->workspace_name, workspace_name, length + 1U);
    umi_studio_document_state_init(&sync->documents);
    sync->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_document_sync_refresh(
    UmiStudioRuntimeDocumentSync *sync,
    UmiStudioRuntimeBindings *bindings)
{
    UmiStatus status;

    if (sync == NULL || bindings == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bindings_validate(bindings);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_document_state_refresh(
        &sync->documents,
        bindings->documents);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_window_title_build(
        "Umicom Studio",
        sync->workspace_name,
        &sync->documents,
        &sync->window_title);
    if (status != UMI_STATUS_OK) return status;

    if (sync->documents.has_active) {
        status = umi_studio_surface_activate_kind(
            bindings,
            UMI_STUDIO_SURFACE_EDITOR,
            1,
            1);
        if (status != UMI_STATUS_OK) return status;
    }

    sync->revision += 1U;
    return UMI_STATUS_OK;
}
