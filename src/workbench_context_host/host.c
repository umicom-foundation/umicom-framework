/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/host.c
 *
 * PURPOSE:
 *   Implement application-facing context-host lifecycle, profile application, publication, observation and navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/host.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UmiWorkbenchContextHostConfig umi_workbench_context_host_config_default(void)
{
    UmiWorkbenchContextHostConfig config;
    memset(&config, 0, sizeof(config));
    config.structure_size = (uint32_t)sizeof(config);
    config.host_id = "workbench";
    config.application_id = "org.umicom.application";
    config.observer_panel_id = "workbench.observer";
    return config;
}

UmiStatus umi_workbench_context_host_create(
    const UmiWorkbenchContextHostConfig *config,
    UmiWorkbenchContextLinkService *link_service,
    UmiWorkbenchContextHost **out_host)
{
    UmiWorkbenchContextHostConfig effective;
    UmiWorkbenchContextHost *host;
    UmiStatus status;

    if (out_host == NULL || link_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_host = NULL;

    effective = config != NULL
        ? *config
        : umi_workbench_context_host_config_default();
    if (effective.structure_size < sizeof(effective) ||
        effective.host_id == NULL ||
        effective.application_id == NULL ||
        effective.observer_panel_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    host = (UmiWorkbenchContextHost *)calloc(1U, sizeof(*host));
    if (host == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    host->link_service = link_service;
    host->revision = 1U;
    umi_workbench_context_host_endpoint_registry_init(&host->endpoints);
    umi_workbench_context_host_inbox_registry_init(&host->inboxes);
    umi_workbench_context_host_observer_init(&host->observer);
    umi_workbench_context_host_metrics_init(&host->metrics);

    status = umi_workbench_context_host_copy_text(
        host->host_id,
        sizeof(host->host_id),
        effective.host_id);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_context_host_copy_text(
            host->application_id,
            sizeof(host->application_id),
            effective.application_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_context_host_copy_text(
            host->observer_panel_id,
            sizeof(host->observer_panel_id),
            effective.observer_panel_id);
    }
    if (status != UMI_STATUS_OK) {
        umi_workbench_context_host_destroy(host);
        return status;
    }

    *out_host = host;
    return UMI_STATUS_OK;
}

void umi_workbench_context_host_destroy(UmiWorkbenchContextHost *host)
{
    if (host == NULL) return;
    umi_workbench_context_host_inbox_registry_destroy(&host->inboxes);
    umi_workbench_context_host_endpoint_registry_destroy(&host->endpoints);
    free(host);
}

UmiStatus umi_workbench_context_host_apply_profile(
    UmiWorkbenchContextHost *host,
    const UmiWorkbenchContextHostProfile *profile)
{
    UmiStatus status;
    if (host == NULL || profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_context_host_profile_apply(
        profile,
        host->link_service,
        &host->endpoints,
        host->active_group_id,
        sizeof(host->active_group_id));
    if (status != UMI_STATUS_OK) {
        ++host->metrics.error_count;
        return status;
    }

    host->metrics.endpoint_registration_count +=
        (uint64_t)profile->endpoint_count;
    ++host->metrics.profile_apply_count;
    ++host->metrics.revision;
    ++host->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_set_active_group(
    UmiWorkbenchContextHost *host,
    const char *group_id)
{
    UmiStatus status;
    if (host == NULL || group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_workbench_context_link_group_catalogue_find_const(
            &host->link_service->groups,
            group_id) == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(host->active_group_id, group_id) == 0) {
        return UMI_STATUS_OK;
    }

    status = umi_workbench_context_host_copy_text(
        host->active_group_id,
        sizeof(host->active_group_id),
        group_id);
    if (status != UMI_STATUS_OK) return status;

    ++host->revision;
    return UMI_STATUS_OK;
}

const char *umi_workbench_context_host_active_group(
    const UmiWorkbenchContextHost *host)
{
    return host != NULL ? host->active_group_id : NULL;
}

UmiStatus umi_workbench_context_host_assign_endpoint_group(
    UmiWorkbenchContextHost *host,
    const char *endpoint_id,
    const char *group_id,
    UmiWorkbenchContextLinkMode mode)
{
    UmiWorkbenchContextHostEndpoint *endpoint;
    char binding_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    int written;
    UmiStatus status;

    if (host == NULL || endpoint_id == NULL || group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_workbench_context_link_group_catalogue_find_const(
            &host->link_service->groups,
            group_id) == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    endpoint = umi_workbench_context_host_endpoint_registry_find(
        &host->endpoints,
        endpoint_id);
    if (endpoint == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_workbench_context_host_endpoint_set_group(
        endpoint,
        group_id,
        mode);
    if (status != UMI_STATUS_OK) return status;

    written = snprintf(
        binding_id,
        sizeof(binding_id),
        "%s.binding",
        endpoint->endpoint_id);
    if (written < 0 || (size_t)written >= sizeof(binding_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_workbench_context_link_service_bind_panel(
        host->link_service,
        binding_id,
        endpoint->panel_id,
        endpoint->application_id,
        group_id,
        UMI_CONTEXT_KIND_GENERIC,
        mode);
    if (status == UMI_STATUS_OK) ++host->revision;
    return status;
}

static const UmiWorkbenchContextHostEndpoint *find_source(
    const UmiWorkbenchContextHost *host,
    const char *source_panel_id)
{
    if (source_panel_id == NULL) return NULL;
    return umi_workbench_context_host_endpoint_registry_find_panel(
        &host->endpoints,
        source_panel_id);
}

UmiStatus umi_workbench_context_host_publish(
    UmiWorkbenchContextHost *host,
    const char *group_id,
    const char *source_panel_id,
    const UmiContextPayload *payload,
    uint64_t now_ms)
{
    const UmiWorkbenchContextHostEndpoint *source;
    const char *effective_group;
    UmiWorkbenchContextLinkDeliveryBatch batch;
    UmiStatus status;

    if (host == NULL || payload == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (host->suspended) return UMI_STATUS_BUSY;

    effective_group =
        group_id != NULL && group_id[0] != '\0'
        ? group_id
        : host->active_group_id;
    if (effective_group == NULL || effective_group[0] == '\0') {
        return UMI_STATUS_NOT_FOUND;
    }

    source = find_source(host, source_panel_id);
    if (source_panel_id != NULL && source_panel_id[0] != '\0' &&
        (source == NULL ||
         !umi_workbench_context_host_endpoint_publishes(
             source,
             payload->kind))) {
        ++host->metrics.error_count;
        return UMI_STATUS_PERMISSION_DENIED;
    }

    status = umi_workbench_context_link_service_publish(
        host->link_service,
        effective_group,
        source_panel_id,
        payload,
        now_ms,
        &batch);
    if (status != UMI_STATUS_OK) {
        ++host->metrics.error_count;
        return status;
    }

    status = umi_workbench_context_host_dispatch(
        &host->endpoints,
        &host->inboxes,
        &host->metrics,
        &batch,
        source_panel_id,
        payload,
        now_ms);
    if (status != UMI_STATUS_OK) return status;

    if (source != NULL) {
        UmiWorkbenchContextHostEndpoint *mutable_source =
            umi_workbench_context_host_endpoint_registry_find(
                &host->endpoints,
                source->endpoint_id);
        if (mutable_source != NULL) {
            ++mutable_source->publish_count;
            ++mutable_source->revision;
        }
    }

    ++host->metrics.publication_count;
    ++host->metrics.revision;
    ++host->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_pop_delivery(
    UmiWorkbenchContextHost *host,
    const char *endpoint_id,
    UmiWorkbenchContextHostDelivery *out_delivery)
{
    UmiWorkbenchContextHostInbox *inbox;
    UmiStatus status;

    if (host == NULL || endpoint_id == NULL ||
        out_delivery == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    inbox = umi_workbench_context_host_inbox_registry_find(
        &host->inboxes,
        endpoint_id);
    if (inbox == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_workbench_context_host_inbox_pop(
        inbox,
        out_delivery);
    if (status == UMI_STATUS_OK) {
        UmiWorkbenchContextHostEndpoint *endpoint =
            umi_workbench_context_host_endpoint_registry_find(
                &host->endpoints,
                endpoint_id);
        if (endpoint != NULL) {
            ++endpoint->delivery_count;
            ++endpoint->revision;
        }
        ++host->metrics.delivery_consumed_count;
        ++host->metrics.revision;
        ++host->revision;
    }
    return status;
}

UmiStatus umi_workbench_context_host_clear_inbox(
    UmiWorkbenchContextHost *host,
    const char *endpoint_id)
{
    UmiWorkbenchContextHostInbox *inbox;
    if (host == NULL || endpoint_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    inbox = umi_workbench_context_host_inbox_registry_find(
        &host->inboxes,
        endpoint_id);
    if (inbox == NULL) return UMI_STATUS_NOT_FOUND;
    umi_workbench_context_host_inbox_clear(inbox);
    ++host->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_observe(
    UmiWorkbenchContextHost *host,
    const UmiWorkbenchContextHostObservation *observation)
{
    UmiContextPayload payload;
    char context_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    uint32_t changes;
    int written;
    UmiStatus status;

    if (host == NULL || observation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    changes = umi_workbench_context_host_observer_push(
        &host->observer,
        observation);
    ++host->metrics.observation_count;
    if (changes == 0U) {
        ++host->metrics.duplicate_observation_count;
        ++host->metrics.revision;
        return UMI_STATUS_OK;
    }

    written = snprintf(
        context_id,
        sizeof(context_id),
        "%s-observation-%llu",
        host->host_id,
        (unsigned long long)observation->source_revision);
    if (written < 0 || (size_t)written >= sizeof(context_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_workbench_context_host_payload_from_observation(
        &payload,
        context_id,
        host->application_id,
        host->observer_panel_id,
        observation);
    if (status != UMI_STATUS_OK) return status;

    status = umi_workbench_context_host_publish(
        host,
        NULL,
        NULL,
        &payload,
        observation->observed_at_ms);
    if (status == UMI_STATUS_NOT_FOUND) {
        /*
         * An observation before a profile establishes an active group is not
         * a runtime error; it simply has nowhere to route yet.
         */
        return UMI_STATUS_OK;
    }
    return status;
}

UmiStatus umi_workbench_context_host_observe_workbench(
    UmiWorkbenchContextHost *host,
    const UmiUiWorkbench *workbench,
    uint64_t now_ms)
{
    UmiWorkbenchContextHostObservation observation;
    UmiStatus status;

    if (host == NULL || workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_host_observation_from_workbench(
        workbench,
        now_ms,
        &observation);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_context_host_observe(
        host,
        &observation);
}

static UmiStatus navigate(
    UmiWorkbenchContextHost *host,
    bool forward,
    uint64_t now_ms)
{
    const UmiWorkbenchContextLinkActiveSlot *slot;
    UmiWorkbenchContextLinkDeliveryBatch batch;
    UmiStatus status;

    if (host == NULL || host->active_group_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = forward
        ? umi_workbench_context_link_service_restore_next(
            host->link_service,
            host->active_group_id,
            &batch)
        : umi_workbench_context_link_service_restore_previous(
            host->link_service,
            host->active_group_id,
            &batch);
    if (status != UMI_STATUS_OK) return status;

    slot = umi_workbench_context_link_service_current(
        host->link_service,
        host->active_group_id);
    if (slot == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_workbench_context_host_dispatch(
        &host->endpoints,
        &host->inboxes,
        &host->metrics,
        &batch,
        NULL,
        &slot->payload,
        now_ms);
    if (status == UMI_STATUS_OK) {
        ++host->metrics.navigation_count;
        ++host->metrics.revision;
        ++host->revision;
    }
    return status;
}

UmiStatus umi_workbench_context_host_back(
    UmiWorkbenchContextHost *host,
    uint64_t now_ms)
{
    return navigate(host, false, now_ms);
}

UmiStatus umi_workbench_context_host_forward(
    UmiWorkbenchContextHost *host,
    uint64_t now_ms)
{
    return navigate(host, true, now_ms);
}

UmiStatus umi_workbench_context_host_pin(
    UmiWorkbenchContextHost *host,
    const char *pin_id,
    uint64_t now_ms)
{
    UmiStatus status;
    if (host == NULL || pin_id == NULL ||
        host->active_group_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_context_link_service_pin_current(
        host->link_service,
        pin_id,
        host->active_group_id,
        now_ms);
    if (status == UMI_STATUS_OK) {
        ++host->metrics.pin_count;
        ++host->metrics.revision;
        ++host->revision;
    }
    return status;
}

void umi_workbench_context_host_set_suspended(
    UmiWorkbenchContextHost *host,
    bool suspended)
{
    if (host == NULL) return;
    host->suspended = suspended;
    umi_workbench_context_link_service_set_suspended(
        host->link_service,
        suspended);
    ++host->revision;
}
