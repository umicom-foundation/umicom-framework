/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/selection_publisher.c
 *
 * PURPOSE:
 *   Publish generic selection contexts from trees, tables, inspectors and list views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/selection_publisher.h"

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
 * Provide the workbench context host publish selection operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_host_publish_selection(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * selection_id,
    const char * selection_type,
    const char * primary_id,
    const char * secondary_id,
    uint64_t index,
    uint64_t count,
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
        selection_id == NULL ||
        selection_type == NULL ||
        primary_id == NULL ||
        secondary_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_payload_init(
        &payload, UMI_CONTEXT_KIND_SELECTION, context_id, "org.umicom.context.selection");
    status = set_source_identity(&payload, host, source_panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_selection_context_set_selection_id(&payload.domain.selection, selection_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_selection_context_set_selection_type(&payload.domain.selection, selection_type);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_selection_context_set_primary_id(&payload.domain.selection, primary_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_selection_context_set_secondary_id(&payload.domain.selection, secondary_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_selection_context_set_index(&payload.domain.selection, index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_selection_context_set_count(&payload.domain.selection, count);
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
