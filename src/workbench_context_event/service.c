/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_event/service.c
 *
 * PURPOSE:
 *   Implement source-authorised event ingestion, duplicate coalescing and typed-context publication.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_event/service.h"

#include <stdlib.h>
#include <string.h>

UmiStatus umi_workbench_context_event_service_create(
    UmiWorkbenchContextHost *host,
    UmiWorkbenchContextEventService **out_service)
{
    UmiWorkbenchContextEventService *service;
    if (host == NULL || out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiWorkbenchContextEventService *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->host = host;
    umi_workbench_context_event_source_registry_init(&service->sources);
    umi_workbench_context_event_queue_init(&service->queue);
    umi_workbench_context_event_coalescer_init(
        &service->coalescer,
        UMI_WORKBENCH_CONTEXT_EVENT_COALESCE_BY_SUBJECT,
        40U);
    service->sequence = 1U;
    service->revision = 1U;
    service->metrics.revision = 1U;
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_workbench_context_event_service_destroy(
    UmiWorkbenchContextEventService *service)
{
    if (service == NULL) return;
    umi_workbench_context_event_queue_destroy(&service->queue);
    umi_workbench_context_event_source_registry_destroy(&service->sources);
    free(service);
}

UmiStatus umi_workbench_context_event_service_register_source(
    UmiWorkbenchContextEventService *service,
    const UmiWorkbenchContextEventSourceDescriptor *descriptor)
{
    UmiStatus status;
    if (service == NULL || descriptor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_source_registry_upsert(
        &service->sources, descriptor);
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

UmiStatus umi_workbench_context_event_service_submit(
    UmiWorkbenchContextEventService *service,
    UmiWorkbenchContextEvent *event)
{
    const UmiWorkbenchContextEventSourceDescriptor *source;
    UmiStatus status;
    if (service == NULL || event == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    ++service->metrics.submitted_count;
    if (service->suspended) {
        ++service->metrics.rejected_count;
        return UMI_STATUS_BUSY;
    }

    source = umi_workbench_context_event_source_registry_find(
        &service->sources, event->source_id);
    if (source == NULL) {
        ++service->metrics.rejected_count;
        return UMI_STATUS_PERMISSION_DENIED;
    }

    event->sequence = ++service->sequence;
    event->state = UMI_WORKBENCH_CONTEXT_EVENT_ACCEPTED;
    (void)umi_workbench_context_event_refresh_hash(event);

    if (!umi_workbench_context_event_coalescer_accept(
            &service->coalescer, event)) {
        event->state = UMI_WORKBENCH_CONTEXT_EVENT_COALESCED;
        ++service->metrics.coalesced_count;
        ++service->metrics.revision;
        return UMI_STATUS_OK;
    }

    status = umi_workbench_context_event_queue_push(
        &service->queue, event);
    if (status != UMI_STATUS_OK) {
        ++service->metrics.dropped_count;
        ++service->metrics.revision;
        return status;
    }
    ++service->metrics.accepted_count;
    ++service->metrics.revision;
    ++service->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_event_service_pump(
    UmiWorkbenchContextEventService *service,
    size_t max_events,
    size_t *out_processed)
{
    size_t processed = 0U;
    UmiStatus status = UMI_STATUS_OK;
    if (service == NULL || out_processed == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    while (service->queue.count > 0U &&
           (max_events == 0U || processed < max_events)) {
        UmiWorkbenchContextEvent event;
        UmiContextPayload payload;
        const UmiWorkbenchContextEventMetadata *group;

        status = umi_workbench_context_event_queue_pop(
            &service->queue, &event);
        if (status != UMI_STATUS_OK) break;

        status = umi_workbench_context_event_map_payload(
            &event, &payload);
        if (status != UMI_STATUS_OK) {
            ++service->metrics.mapping_failure_count;
            ++service->metrics.rejected_count;
            ++processed;
            continue;
        }

        group = umi_workbench_context_event_find_metadata(
            &event, "group-id");
        if (group != NULL) {
            status = umi_workbench_context_host_publish(
                service->host,
                group->value,
                event.panel_id,
                &payload,
                event.timestamp_ms);
        } else {
            const UmiWorkbenchContextHostEndpoint *endpoint =
                umi_workbench_context_host_endpoint_registry_find_panel(
                    &service->host->endpoints,
                    event.panel_id);
            status = umi_workbench_context_host_publish(
                service->host,
                endpoint != NULL && endpoint->group_id[0] != '\0'
                    ? endpoint->group_id
                    : NULL,
                event.panel_id,
                &payload,
                event.timestamp_ms);
        }
        if (status != UMI_STATUS_OK) {
            ++service->metrics.publication_failure_count;
            ++service->metrics.rejected_count;
            ++processed;
            continue;
        }

        ++service->metrics.routed_count;
        ++processed;
    }

    service->metrics.dropped_count = service->queue.dropped_count;
    ++service->metrics.revision;
    ++service->revision;
    *out_processed = processed;
    return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_OK : status;
}

void umi_workbench_context_event_service_set_suspended(
    UmiWorkbenchContextEventService *service,
    bool suspended)
{
    if (service == NULL) return;
    service->suspended = suspended;
    ++service->revision;
}
