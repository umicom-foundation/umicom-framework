/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/source_location_publisher.c
 *
 * PURPOSE:
 *   Publish canonical source-location contexts from editors, diagnostics, search and debugger views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/source_location_publisher.h"

#include <string.h>

/*
 * Provide the set source identity operation used by this module and its client
 * applications.
 */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source_panel_id != NULL && source_panel_id[0] != '\0') {
        return umi_context_copy_text(
            payload->identity.source_panel_id,
            sizeof(payload->identity.source_panel_id),
            source_panel_id);
    }
    payload->identity.source_panel_id[0] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context host publish source location operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_host_publish_source_location(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * workspace_id,
    const char * file_path,
    const char * symbol,
    uint32_t line,
    uint32_t column,
    uint32_t selection_length,
    uint64_t now_ms)
{
    UmiContextPayload payload;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL ||
        context_id == NULL ||
        workspace_id == NULL ||
        file_path == NULL ||
        symbol == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_payload_init(
        &payload, UMI_CONTEXT_KIND_SOURCE_LOCATION, context_id, "org.umicom.context.source-location");
    status = set_source_identity(&payload, host, source_panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_source_location_context_set_workspace_id(&payload.domain.source_location, workspace_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_source_location_context_set_file_path(&payload.domain.source_location, file_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_source_location_context_set_symbol(&payload.domain.source_location, symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_source_location_context_set_line(&payload.domain.source_location, line);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_source_location_context_set_column(&payload.domain.source_location, column);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_source_location_context_set_selection_length(&payload.domain.source_location, selection_length);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    payload.audit.published_at_ms = now_ms;
    umi_context_payload_refresh_hash(&payload);
    status = umi_context_payload_validate(&payload);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_context_host_publish(
        host, group_id, source_panel_id, &payload, now_ms);
}
