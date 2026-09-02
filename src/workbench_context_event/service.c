/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_event/service.c
 *
 * PURPOSE:
 *   Implement source-authorised event ingestion, duplicate coalescing and typed-context publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_event/service.h"

#include <stdlib.h>
#include <string.h>

/*
 * Initialise workbench context event service from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_context_event_service_create(
    UmiWorkbenchContextHost *host,
    UmiWorkbenchContextEventService **out_service)
{
    UmiWorkbenchContextEventService *service;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiWorkbenchContextEventService *)calloc(1U, sizeof(*service));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Release or reset state held by workbench context event service so the same storage can
 * be reused safely.
 */
void umi_workbench_context_event_service_destroy(
    UmiWorkbenchContextEventService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    umi_workbench_context_event_queue_destroy(&service->queue);
    umi_workbench_context_event_source_registry_destroy(&service->sources);
    free(service);
}

/*
 * Provide the workbench context event service register source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_service_register_source(
    UmiWorkbenchContextEventService *service,
    const UmiWorkbenchContextEventSourceDescriptor *descriptor)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || descriptor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_source_registry_upsert(
        &service->sources, descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

/*
 * Provide the workbench context event service submit operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_service_submit(
    UmiWorkbenchContextEventService *service,
    UmiWorkbenchContextEvent *event)
{
    const UmiWorkbenchContextEventSourceDescriptor *source;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || event == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    ++service->metrics.submitted_count;
    /* Apply this branch only when its contract condition is satisfied. */
    if (service->suspended) {
        ++service->metrics.rejected_count;
        return UMI_STATUS_BUSY;
    }

    source = umi_workbench_context_event_source_registry_find(
        &service->sources, event->source_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) {
        ++service->metrics.rejected_count;
        return UMI_STATUS_PERMISSION_DENIED;
    }

    event->sequence = ++service->sequence;
    event->state = UMI_WORKBENCH_CONTEXT_EVENT_ACCEPTED;
    (void)umi_workbench_context_event_refresh_hash(event);

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_workbench_context_event_coalescer_accept(
            &service->coalescer, event)) {
        event->state = UMI_WORKBENCH_CONTEXT_EVENT_COALESCED;
        ++service->metrics.coalesced_count;
        ++service->metrics.revision;
        return UMI_STATUS_OK;
    }

    status = umi_workbench_context_event_queue_push(
        &service->queue, event);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the workbench context event service pump operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_event_service_pump(
    UmiWorkbenchContextEventService *service,
    size_t max_events,
    size_t *out_processed)
{
    size_t processed = 0U;
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_processed == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (service->queue.count > 0U &&
           (max_events == 0U || processed < max_events)) {
        UmiWorkbenchContextEvent event;
        UmiContextPayload payload;
        const UmiWorkbenchContextEventMetadata *group;

        status = umi_workbench_context_event_queue_pop(
            &service->queue, &event);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;

        status = umi_workbench_context_event_map_payload(
            &event, &payload);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            ++service->metrics.mapping_failure_count;
            ++service->metrics.rejected_count;
            ++processed;
            continue;
        }

        group = umi_workbench_context_event_find_metadata(
            &event, "group-id");
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (group != NULL) {
            status = umi_workbench_context_host_publish(
                service->host,
                group->value,
                event.panel_id,
                &payload,
                event.timestamp_ms);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
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
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the workbench context event service set suspended operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_service_set_suspended(
    UmiWorkbenchContextEventService *service,
    bool suspended)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    service->suspended = suspended;
    ++service->revision;
}
