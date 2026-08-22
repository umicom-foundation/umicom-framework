/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/project_publisher.c
 *
 * PURPOSE:
 *   Publish canonical project contexts from Studio project, build and source-control views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/project_publisher.h"

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

UmiStatus umi_workbench_context_host_publish_project(
    UmiWorkbenchContextHost * host,
    const char * group_id,
    const char * source_panel_id,
    const char * context_id,
    const char * project_id,
    const char * repository_id,
    const char * root_path,
    const char * target_id,
    const char * configuration_id,
    const char * language_id,
    uint64_t now_ms)
{
    UmiContextPayload payload;
    UmiStatus status;
    if (host == NULL ||
        context_id == NULL ||
        project_id == NULL ||
        repository_id == NULL ||
        root_path == NULL ||
        target_id == NULL ||
        configuration_id == NULL ||
        language_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_context_payload_init(
        &payload, UMI_CONTEXT_KIND_PROJECT, context_id, "org.umicom.context.project");
    status = set_source_identity(&payload, host, source_panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_context_set_project_id(&payload.domain.project, project_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_context_set_repository_id(&payload.domain.project, repository_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_context_set_root_path(&payload.domain.project, root_path);
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_context_set_target_id(&payload.domain.project, target_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_context_set_configuration_id(&payload.domain.project, configuration_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_context_set_language_id(&payload.domain.project, language_id);
    if (status != UMI_STATUS_OK) return status;
    payload.audit.published_at_ms = now_ms;
    umi_context_payload_refresh_hash(&payload);
    status = umi_context_payload_validate(&payload);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_context_host_publish(
        host, group_id, source_panel_id, &payload, now_ms);
}
