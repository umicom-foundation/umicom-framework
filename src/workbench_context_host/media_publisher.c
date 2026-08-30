/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/media_publisher.c
 *
 * PURPOSE:
 *   Publish canonical media contexts for future audio, video and generated-content workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/media_publisher.h"

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

UmiStatus umi_workbench_context_host_publish_media(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * asset_id,
    const char * timeline_id,
    const char * track_id,
    uint64_t timecode_ms,
    uint64_t duration_ms,
    const char * media_type,
    uint64_t now_ms)
{
    UmiContextPayload payload;
    UmiStatus status;
    if (host == NULL ||
        context_id == NULL ||
        asset_id == NULL ||
        timeline_id == NULL ||
        track_id == NULL ||
        media_type == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_payload_init(
        &payload, UMI_CONTEXT_KIND_MEDIA, context_id, "org.umicom.context.media");
    status = set_source_identity(&payload, host, source_panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_media_context_set_asset_id(&payload.domain.media, asset_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_media_context_set_timeline_id(&payload.domain.media, timeline_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_media_context_set_track_id(&payload.domain.media, track_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_media_context_set_timecode_ms(&payload.domain.media, timecode_ms);
    if (status != UMI_STATUS_OK) return status;
    status = umi_media_context_set_duration_ms(&payload.domain.media, duration_ms);
    if (status != UMI_STATUS_OK) return status;
    status = umi_media_context_set_media_type(&payload.domain.media, media_type);
    if (status != UMI_STATUS_OK) return status;
    payload.audit.published_at_ms = now_ms;
    umi_context_payload_refresh_hash(&payload);
    status = umi_context_payload_validate(&payload);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_context_host_publish(
        host, group_id, source_panel_id, &payload, now_ms);
}
