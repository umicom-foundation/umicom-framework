/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_event/payload_mapper.c
 *
 * PURPOSE:
 *   Map project, source, diagnostic, test, AI, trading, workspace and generic interaction events into typed contexts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_event/payload_mapper.h"

#include "umicom/workbench_context_host/payload.h"
#include "umicom/workbench_context_link/payload_builder.h"

#include <string.h>

/* Provide the metadata operation used by this module and its client applications. */
static const char *metadata(
    const UmiWorkbenchContextEvent *event,
    const char *name,
    const char *fallback)
{
    const UmiWorkbenchContextEventMetadata *item =
        umi_workbench_context_event_find_metadata(event, name);
    return item != NULL ? item->value : fallback;
}

/* Provide the apply identity operation used by this module and its client applications. */
static UmiStatus apply_identity(
    UmiContextPayload *payload,
    const UmiWorkbenchContextEvent *event)
{
    UmiStatus status;
    status = umi_context_copy_text(
        payload->identity.source_application_id,
        sizeof(payload->identity.source_application_id),
        event->application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_context_copy_text(
        payload->identity.source_panel_id,
        sizeof(payload->identity.source_panel_id),
        event->panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (event->workspace_id[0] != '\0') {
        status = umi_context_copy_text(
            payload->audit.workspace_id,
            sizeof(payload->audit.workspace_id),
            event->workspace_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    payload->identity.sequence = event->sequence;
    payload->identity.created_at_ms = event->timestamp_ms;
    payload->audit.published_at_ms = event->timestamp_ms;
    return UMI_STATUS_OK;
}

/*
 * Provide the add event metadata operation used by this module and its client
 * applications.
 */
static UmiStatus add_event_metadata(
    UmiContextPayload *payload,
    const UmiWorkbenchContextEvent *event)
{
    size_t index;
    UmiStatus status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < event->metadata_count; ++index) {
        status = umi_workbench_context_host_payload_add_text(
            payload,
            event->metadata[index].name,
            event->metadata[index].value);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the map source location operation used by this module and its client
 * applications.
 */
static UmiStatus map_source_location(
    const UmiWorkbenchContextEvent *event,
    UmiContextPayload *payload)
{
    UmiStatus status = umi_workbench_context_link_build_source_location(
        payload,
        event->event_id,
        event->application_id,
        event->panel_id,
        event->workspace_id[0] != '\0' ? event->workspace_id : "workspace",
        event->path[0] != '\0' ? event->path : "unknown",
        event->line,
        event->column);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)umi_source_location_context_set_symbol(
        &payload->domain.source_location,
        metadata(event, "symbol", ""));
    (void)umi_source_location_context_set_selection_length(
        &payload->domain.source_location,
        event->selection_length);
    return UMI_STATUS_OK;
}

/* Provide the map project operation used by this module and its client applications. */
static UmiStatus map_project(
    const UmiWorkbenchContextEvent *event,
    UmiContextPayload *payload)
{
    UmiStatus status;
    umi_context_payload_init(
        payload,
        UMI_CONTEXT_KIND_PROJECT,
        event->event_id,
        "org.umicom.context.project");
    status = apply_identity(payload, event);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_context_set_project_id(
        &payload->domain.project,
        event->subject_id[0] != '\0' ? event->subject_id : event->event_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_context_set_repository_id(
        &payload->domain.project,
        metadata(event, "repository-id", ""));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_context_set_root_path(
        &payload->domain.project,
        event->path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)umi_project_context_set_target_id(
        &payload->domain.project,
        metadata(event, "target-id", ""));
    (void)umi_project_context_set_configuration_id(
        &payload->domain.project,
        metadata(event, "configuration-id", ""));
    (void)umi_project_context_set_language_id(
        &payload->domain.project,
        metadata(event, "language-id", ""));
    return UMI_STATUS_OK;
}

/* Provide the map instrument operation used by this module and its client applications. */
static UmiStatus map_instrument(
    const UmiWorkbenchContextEvent *event,
    UmiContextPayload *payload)
{
    return umi_workbench_context_link_build_instrument(
        payload,
        event->event_id,
        event->application_id,
        event->panel_id,
        event->subject_id,
        metadata(event, "symbol", event->subject_id),
        metadata(event, "venue", ""));
}

/* Provide the map selection operation used by this module and its client applications. */
static UmiStatus map_selection(
    const UmiWorkbenchContextEvent *event,
    UmiContextPayload *payload)
{
    return umi_workbench_context_link_build_selection(
        payload,
        event->event_id,
        event->application_id,
        event->panel_id,
        event->subject_id[0] != '\0' ? event->subject_id : event->event_id,
        umi_workbench_context_event_kind_text(event->kind),
        event->secondary_id[0] != '\0'
            ? event->secondary_id
            : (event->subject_id[0] != '\0' ? event->subject_id : event->event_id));
}

/* Provide the map account operation used by this module and its client applications. */
static UmiStatus map_account(
    const UmiWorkbenchContextEvent *event,
    UmiContextPayload *payload)
{
    UmiStatus status;
    umi_context_payload_init(
        payload,
        UMI_CONTEXT_KIND_ACCOUNT,
        event->event_id,
        "org.umicom.context.account");
    status = apply_identity(payload, event);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_account_context_set_account_id(
        &payload->domain.account, event->subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)umi_account_context_set_organisation_id(
        &payload->domain.account, metadata(event, "organisation-id", ""));
    (void)umi_account_context_set_book_id(
        &payload->domain.account, metadata(event, "book-id", ""));
    (void)umi_account_context_set_currency(
        &payload->domain.account, metadata(event, "currency", ""));
    (void)umi_account_context_set_account_type(
        &payload->domain.account, metadata(event, "account-type", ""));
    (void)umi_account_context_set_environment(
        &payload->domain.account, metadata(event, "environment", ""));
    return UMI_STATUS_OK;
}

/* Provide the map trade operation used by this module and its client applications. */
static UmiStatus map_trade(
    const UmiWorkbenchContextEvent *event,
    UmiContextPayload *payload)
{
    UmiStatus status;
    umi_context_payload_init(
        payload,
        UMI_CONTEXT_KIND_TRADE,
        event->event_id,
        "org.umicom.context.trade");
    status = apply_identity(payload, event);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_trade_context_set_trade_id(
        &payload->domain.trade, event->subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)umi_trade_context_set_source_system(
        &payload->domain.trade, metadata(event, "source-system", ""));
    (void)umi_trade_context_set_product_type(
        &payload->domain.trade, metadata(event, "product-type", ""));
    (void)umi_trade_context_set_book_id(
        &payload->domain.trade, metadata(event, "book-id", ""));
    (void)umi_trade_context_set_counterparty_id(
        &payload->domain.trade, metadata(event, "counterparty-id", ""));
    (void)umi_trade_context_set_version(
        &payload->domain.trade, event->unsigned_value);
    return UMI_STATUS_OK;
}

/* Provide the map workspace operation used by this module and its client applications. */
static UmiStatus map_workspace(
    const UmiWorkbenchContextEvent *event,
    UmiContextPayload *payload)
{
    UmiStatus status;
    umi_context_payload_init(
        payload,
        UMI_CONTEXT_KIND_WORKSPACE,
        event->event_id,
        "org.umicom.context.workspace");
    status = apply_identity(payload, event);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workspace_context_set_workspace_id(
        &payload->domain.workspace,
        event->workspace_id[0] != '\0' ? event->workspace_id : event->subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)umi_workspace_context_set_profile_id(
        &payload->domain.workspace, metadata(event, "profile-id", ""));
    (void)umi_workspace_context_set_root_path(
        &payload->domain.workspace, event->path);
    (void)umi_workspace_context_set_perspective_id(
        &payload->domain.workspace, metadata(event, "perspective-id", ""));
    (void)umi_workspace_context_set_layout_id(
        &payload->domain.workspace, metadata(event, "layout-id", ""));
    (void)umi_workspace_context_set_trusted(
        &payload->domain.workspace, event->boolean_value);
    return UMI_STATUS_OK;
}

/* Provide the map media operation used by this module and its client applications. */
static UmiStatus map_media(
    const UmiWorkbenchContextEvent *event,
    UmiContextPayload *payload)
{
    UmiStatus status;
    umi_context_payload_init(
        payload,
        UMI_CONTEXT_KIND_MEDIA,
        event->event_id,
        "org.umicom.context.media");
    status = apply_identity(payload, event);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_media_context_set_asset_id(
        &payload->domain.media, event->subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)umi_media_context_set_timeline_id(
        &payload->domain.media, metadata(event, "timeline-id", ""));
    (void)umi_media_context_set_track_id(
        &payload->domain.media, metadata(event, "track-id", ""));
    (void)umi_media_context_set_timecode_ms(
        &payload->domain.media, event->unsigned_value);
    (void)umi_media_context_set_duration_ms(
        &payload->domain.media, (uint64_t)(event->signed_value < 0 ? 0 : event->signed_value));
    (void)umi_media_context_set_media_type(
        &payload->domain.media, metadata(event, "media-type", ""));
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context event map payload operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_map_payload(
    const UmiWorkbenchContextEvent *event,
    UmiContextPayload *out_payload)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event == NULL || out_payload == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_validate(event);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Select the behaviour associated with the requested command or state value. */
    switch (event->context_kind) {
    case UMI_CONTEXT_KIND_SOURCE_LOCATION:
        status = map_source_location(event, out_payload);
        break;
    case UMI_CONTEXT_KIND_PROJECT:
        status = map_project(event, out_payload);
        break;
    case UMI_CONTEXT_KIND_INSTRUMENT:
        status = map_instrument(event, out_payload);
        break;
    case UMI_CONTEXT_KIND_ACCOUNT:
        status = map_account(event, out_payload);
        break;
    case UMI_CONTEXT_KIND_TRADE:
        status = map_trade(event, out_payload);
        break;
    case UMI_CONTEXT_KIND_WORKSPACE:
        status = map_workspace(event, out_payload);
        break;
    case UMI_CONTEXT_KIND_MEDIA:
        status = map_media(event, out_payload);
        break;
    case UMI_CONTEXT_KIND_SELECTION:
    case UMI_CONTEXT_KIND_GENERIC:
        status = map_selection(event, out_payload);
        break;
    default:
        status = UMI_STATUS_INVALID_ARGUMENT;
        break;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = apply_identity(out_payload, event);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_event_metadata(out_payload, event);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_context_payload_refresh_hash(out_payload);
    return umi_context_payload_validate(out_payload);
}
