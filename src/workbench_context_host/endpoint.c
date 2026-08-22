/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/endpoint.c
 *
 * PURPOSE:
 *   Implement context-aware endpoint validation, identity, grouping and capability checks.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/endpoint.h"

#include <string.h>

void umi_workbench_context_host_endpoint_init(
    UmiWorkbenchContextHostEndpoint *endpoint,
    const char *endpoint_id)
{
    if (endpoint == NULL) return;
    memset(endpoint, 0, sizeof(*endpoint));
    endpoint->structure_size = (uint32_t)sizeof(*endpoint);
    endpoint->role = UMI_WORKBENCH_CONTEXT_HOST_PANEL_GENERIC;
    endpoint->state = UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_READY;
    endpoint->mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE;
    endpoint->accepted_kinds_mask = UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK;
    endpoint->published_kinds_mask = UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK;
    endpoint->enabled = true;
    endpoint->revision = 1U;
    if (endpoint_id != NULL) {
        (void)umi_workbench_context_host_copy_text(
            endpoint->endpoint_id, sizeof(endpoint->endpoint_id), endpoint_id);
    }
}

UmiStatus umi_workbench_context_host_endpoint_validate(
    const UmiWorkbenchContextHostEndpoint *endpoint)
{
    if (endpoint == NULL || endpoint->structure_size != sizeof(*endpoint)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_context_host_text_is_valid(
            endpoint->endpoint_id, sizeof(endpoint->endpoint_id)) ||
        endpoint->endpoint_id[0] == '\0' ||
        !umi_workbench_context_host_text_is_valid(
            endpoint->panel_id, sizeof(endpoint->panel_id)) ||
        endpoint->panel_id[0] == '\0' ||
        !umi_workbench_context_host_text_is_valid(
            endpoint->application_id, sizeof(endpoint->application_id)) ||
        endpoint->application_id[0] == '\0' ||
        !umi_workbench_context_host_text_is_valid(
            endpoint->display_name, sizeof(endpoint->display_name)) ||
        !umi_workbench_context_host_text_is_valid(
            endpoint->group_id, sizeof(endpoint->group_id))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (endpoint->role < UMI_WORKBENCH_CONTEXT_HOST_PANEL_GENERIC ||
        endpoint->role > UMI_WORKBENCH_CONTEXT_HOST_PANEL_CONTEXT_INSPECTOR) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (endpoint->state < UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_DISABLED ||
        endpoint->state > UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_ERROR) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (endpoint->mode < UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE ||
        endpoint->mode > UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_endpoint_set_identity(
    UmiWorkbenchContextHostEndpoint *endpoint,
    const char *panel_id,
    const char *application_id,
    const char *display_name)
{
    UmiStatus status;
    if (endpoint == NULL || panel_id == NULL || application_id == NULL ||
        display_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_host_copy_text(
        endpoint->panel_id, sizeof(endpoint->panel_id), panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_host_copy_text(
        endpoint->application_id, sizeof(endpoint->application_id), application_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_host_copy_text(
        endpoint->display_name, sizeof(endpoint->display_name), display_name);
    if (status == UMI_STATUS_OK) ++endpoint->revision;
    return status;
}

UmiStatus umi_workbench_context_host_endpoint_set_group(
    UmiWorkbenchContextHostEndpoint *endpoint,
    const char *group_id,
    UmiWorkbenchContextLinkMode mode)
{
    UmiStatus status;
    if (endpoint == NULL || group_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_host_copy_text(
        endpoint->group_id, sizeof(endpoint->group_id), group_id);
    if (status != UMI_STATUS_OK) return status;
    endpoint->mode = mode;
    ++endpoint->revision;
    return UMI_STATUS_OK;
}

bool umi_workbench_context_host_endpoint_accepts(
    const UmiWorkbenchContextHostEndpoint *endpoint,
    UmiContextKind kind)
{
    if (endpoint == NULL || !endpoint->enabled ||
        endpoint->state == UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_DISABLED ||
        endpoint->state == UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_SUSPENDED) {
        return false;
    }
    return umi_workbench_context_host_kind_allowed(
        endpoint->accepted_kinds_mask, kind);
}

bool umi_workbench_context_host_endpoint_publishes(
    const UmiWorkbenchContextHostEndpoint *endpoint,
    UmiContextKind kind)
{
    if (endpoint == NULL || !endpoint->enabled ||
        endpoint->state == UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_DISABLED ||
        endpoint->state == UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_SUSPENDED) {
        return false;
    }
    return umi_workbench_context_host_kind_allowed(
        endpoint->published_kinds_mask, kind);
}
