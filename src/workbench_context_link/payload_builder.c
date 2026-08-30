/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/payload_builder.c
 *
 * PURPOSE:
 *   Construct validated source-location, instrument, project and selection contexts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/payload_builder.h"

#include <string.h>

static UmiStatus set_identity(
    UmiContextPayload *payload,
    const char *application_id,
    const char *panel_id)
{
    UmiStatus status;
    status = umi_context_copy_text(
        payload->identity.source_application_id,
        sizeof(payload->identity.source_application_id),
        application_id);
    if (status != UMI_STATUS_OK) return status;
    return umi_context_copy_text(
        payload->identity.source_panel_id,
        sizeof(payload->identity.source_panel_id),
        panel_id);
}

UmiStatus umi_workbench_context_link_build_source_location(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    const char *file_path,
    uint32_t line,
    uint32_t column)
{
    UmiStatus status;
    if (out_payload == NULL || context_id == NULL || application_id == NULL ||
        panel_id == NULL || workspace_id == NULL || file_path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_payload_init(
        out_payload, UMI_CONTEXT_KIND_SOURCE_LOCATION,
        context_id, "org.umicom.context.source-location");
    status = set_identity(out_payload, application_id, panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_source_location_context_set_workspace_id(
        &out_payload->domain.source_location, workspace_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_source_location_context_set_file_path(
        &out_payload->domain.source_location, file_path);
    if (status != UMI_STATUS_OK) return status;
    (void)umi_source_location_context_set_line(
        &out_payload->domain.source_location, line);
    (void)umi_source_location_context_set_column(
        &out_payload->domain.source_location, column);
    umi_context_payload_refresh_hash(out_payload);
    return umi_context_payload_validate(out_payload);
}

UmiStatus umi_workbench_context_link_build_instrument(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *application_id,
    const char *panel_id,
    const char *instrument_id,
    const char *symbol,
    const char *venue)
{
    UmiStatus status;
    if (out_payload == NULL || context_id == NULL || application_id == NULL ||
        panel_id == NULL || instrument_id == NULL || symbol == NULL || venue == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_payload_init(
        out_payload, UMI_CONTEXT_KIND_INSTRUMENT,
        context_id, "org.umicom.context.instrument");
    status = set_identity(out_payload, application_id, panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_instrument_context_set_instrument_id(
        &out_payload->domain.instrument, instrument_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_instrument_context_set_symbol(
        &out_payload->domain.instrument, symbol);
    if (status != UMI_STATUS_OK) return status;
    status = umi_instrument_context_set_venue(
        &out_payload->domain.instrument, venue);
    if (status != UMI_STATUS_OK) return status;
    umi_context_payload_refresh_hash(out_payload);
    return umi_context_payload_validate(out_payload);
}

UmiStatus umi_workbench_context_link_build_project(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *application_id,
    const char *panel_id,
    const char *project_id,
    const char *root_path,
    const char *language_id)
{
    UmiStatus status;
    if (out_payload == NULL || context_id == NULL || application_id == NULL ||
        panel_id == NULL || project_id == NULL || root_path == NULL ||
        language_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_payload_init(
        out_payload, UMI_CONTEXT_KIND_PROJECT,
        context_id, "org.umicom.context.project");
    status = set_identity(out_payload, application_id, panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_context_set_project_id(
        &out_payload->domain.project, project_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_context_set_root_path(
        &out_payload->domain.project, root_path);
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_context_set_language_id(
        &out_payload->domain.project, language_id);
    if (status != UMI_STATUS_OK) return status;
    umi_context_payload_refresh_hash(out_payload);
    return umi_context_payload_validate(out_payload);
}

UmiStatus umi_workbench_context_link_build_selection(
    UmiContextPayload *out_payload,
    const char *context_id,
    const char *application_id,
    const char *panel_id,
    const char *selection_id,
    const char *selection_type,
    const char *primary_id)
{
    UmiStatus status;
    if (out_payload == NULL || context_id == NULL || application_id == NULL ||
        panel_id == NULL || selection_id == NULL || selection_type == NULL ||
        primary_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_payload_init(
        out_payload, UMI_CONTEXT_KIND_SELECTION,
        context_id, "org.umicom.context.selection");
    status = set_identity(out_payload, application_id, panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_selection_context_set_selection_id(
        &out_payload->domain.selection, selection_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_selection_context_set_selection_type(
        &out_payload->domain.selection, selection_type);
    if (status != UMI_STATUS_OK) return status;
    status = umi_selection_context_set_primary_id(
        &out_payload->domain.selection, primary_id);
    if (status != UMI_STATUS_OK) return status;
    umi_context_payload_refresh_hash(out_payload);
    return umi_context_payload_validate(out_payload);
}
