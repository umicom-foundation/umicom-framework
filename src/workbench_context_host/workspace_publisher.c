/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/workspace_publisher.c
 *
 * PURPOSE:
 *   Publish canonical workspace contexts when workbench profiles, perspectives and layouts change.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/workspace_publisher.h"

#include <string.h>

static UmiStatus set_source_identity(
    UmiContextPayload *payload,
    const UmiWorkbenchContextHost *host,
    const char *source_panel_id)
{
    UmiStatus status;
    status = umi_context_copy_text(
        payload->identity.source_application_id,
        sizeof(payload->identity.source_application_id),
        host->application_id);
    if (status != UMI_STATUS_OK) return status;
    if (source_panel_id != NULL && source_panel_id[0] != '\0') {
        return umi_context_copy_text(
            payload->identity.source_panel_id,
            sizeof(payload->identity.source_panel_id),
            source_panel_id);
    }
    payload->identity.source_panel_id[0] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_publish_workspace(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * workspace_id,
    const char * profile_id,
    const char * root_path,
    const char * perspective_id,
    const char * layout_id,
    bool trusted,
    uint64_t now_ms)
{
    UmiContextPayload payload;
    UmiStatus status;
    if (host == NULL ||
        context_id == NULL ||
        workspace_id == NULL ||
        profile_id == NULL ||
        root_path == NULL ||
        perspective_id == NULL ||
        layout_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_payload_init(
        &payload, UMI_CONTEXT_KIND_WORKSPACE, context_id, "org.umicom.context.workspace");
    status = set_source_identity(&payload, host, source_panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workspace_context_set_workspace_id(&payload.domain.workspace, workspace_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workspace_context_set_profile_id(&payload.domain.workspace, profile_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workspace_context_set_root_path(&payload.domain.workspace, root_path);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workspace_context_set_perspective_id(&payload.domain.workspace, perspective_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workspace_context_set_layout_id(&payload.domain.workspace, layout_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workspace_context_set_trusted(&payload.domain.workspace, trusted);
    if (status != UMI_STATUS_OK) return status;
    payload.audit.published_at_ms = now_ms;
    umi_context_payload_refresh_hash(&payload);
    status = umi_context_payload_validate(&payload);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_context_host_publish(
        host, group_id, source_panel_id, &payload, now_ms);
}
