/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/document_sync.c
 *
 * PURPOSE:
 *   Implement DocumentCoordinator -> editor focus/title synchronization.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/document_sync.h"

#include <string.h>

/*
 * Initialise studio document sync from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_studio_document_sync_init(
    UmiStudioRuntimeDocumentSync *sync,
    const char *workspace_name)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sync == NULL || workspace_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(workspace_name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= sizeof(sync->workspace_name)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(sync, 0, sizeof(*sync));
    (void)memcpy(sync->workspace_name, workspace_name, length + 1U);
    umi_studio_document_state_init(&sync->documents);
    sync->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the studio document sync refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_document_sync_refresh(
    UmiStudioRuntimeDocumentSync *sync,
    UmiStudioRuntimeBindings *bindings)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sync == NULL || bindings == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_bindings_validate(bindings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_document_state_refresh(
        &sync->documents,
        bindings->documents);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_window_title_build(
        "Umicom Studio",
        sync->workspace_name,
        &sync->documents,
        &sync->window_title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this operation only while the related capability or state is available. */
    if (sync->documents.has_active) {
        status = umi_studio_surface_activate_kind(
            bindings,
            UMI_STUDIO_SURFACE_EDITOR,
            1,
            1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    sync->revision += 1U;
    return UMI_STATUS_OK;
}
