/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/service.c
 *
 * PURPOSE:
 *   Implement provider-authorised structured selection publication and the
 *   reusable provider/source/event runtime pipeline used by Umicom products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/service.h"

#include <stdlib.h>

/*
 * Ownership is deliberately explicit.  The Context Host is owned by the
 * application composition, while every service pointer below is owned by this
 * pipeline and is destroyed in reverse construction order.
 */
struct UmiWorkbenchSelectionProviderPipeline {
    UmiWorkbenchContextEventService *events;
    UmiWorkbenchContextSourceService *sources;
    UmiWorkbenchSelectionService *selections;
    UmiWorkbenchSelectionProviderService *providers;
};

UmiStatus umi_workbench_selection_provider_service_create(
    UmiWorkbenchSelectionService *selections,
    UmiWorkbenchSelectionProviderService **out_service)
{
    UmiWorkbenchSelectionProviderService *service;
    if (selections == NULL || out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    service = (UmiWorkbenchSelectionProviderService *)calloc(
        1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->selections = selections;
    umi_workbench_selection_provider_registry_init(&service->registry);
    umi_workbench_selection_provider_history_init(&service->history);
    service->sequence = 1U;
    service->revision = 1U;
    service->metrics.revision = 1U;
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_workbench_selection_provider_service_destroy(
    UmiWorkbenchSelectionProviderService *service)
{
    if (service == NULL) return;
    umi_workbench_selection_provider_history_destroy(&service->history);
    umi_workbench_selection_provider_registry_destroy(&service->registry);
    free(service);
}

UmiStatus umi_workbench_selection_provider_service_register(
    UmiWorkbenchSelectionProviderService *service,
    const UmiWorkbenchSelectionProviderDescriptor *descriptor)
{
    UmiStatus status;
    if (service == NULL || descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_provider_registry_upsert(
        &service->registry, descriptor);
    if (status == UMI_STATUS_OK) {
        ++service->metrics.provider_registration_count;
        ++service->metrics.revision;
        ++service->revision;
    }
    return status;
}

UmiStatus umi_workbench_selection_provider_service_unregister(
    UmiWorkbenchSelectionProviderService *service,
    const char *provider_id)
{
    UmiStatus status;
    if (service == NULL || provider_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_provider_registry_remove(
        &service->registry, provider_id);
    if (status == UMI_STATUS_OK) {
        ++service->metrics.provider_removal_count;
        ++service->metrics.revision;
        ++service->revision;
    }
    return status;
}

UmiStatus umi_workbench_selection_provider_service_publish(
    UmiWorkbenchSelectionProviderService *service,
    const char *provider_id,
    UmiWorkbenchSelection *selection,
    UmiWorkbenchContextSourceKind source_kind,
    UmiWorkbenchContextSourceTrigger trigger)
{
    const UmiWorkbenchSelectionProviderDescriptor *descriptor;
    UmiStatus status;
    uint64_t before_duplicates;
    if (service == NULL || provider_id == NULL || selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    ++service->metrics.selection_submit_count;
    if (service->suspended) {
        ++service->metrics.rejection_count;
        ++service->metrics.revision;
        return UMI_STATUS_BUSY;
    }

    descriptor = umi_workbench_selection_provider_registry_find(
        &service->registry, provider_id);
    if (descriptor == NULL || !descriptor->enabled ||
        descriptor->state == UMI_WORKBENCH_SELECTION_PROVIDER_SUSPENDED ||
        descriptor->state == UMI_WORKBENCH_SELECTION_PROVIDER_FAILED) {
        ++service->metrics.rejection_count;
        ++service->metrics.revision;
        return UMI_STATUS_PERMISSION_DENIED;
    }

    if (selection->application_id[0] == '\0') {
        status = umi_workbench_selection_set_origin(
            selection,
            descriptor->application_id,
            descriptor->panel_id,
            selection->workspace_id);
        if (status != UMI_STATUS_OK) return status;
    }

    if (selection->group_hint[0] == '\0' &&
        descriptor->default_group_id[0] != '\0') {
        status = umi_workbench_selection_set_group_hint(
            selection,
            descriptor->default_group_id);
        if (status != UMI_STATUS_OK) return status;
    }

    before_duplicates = service->selections->metrics.duplicate_count;
    status = umi_workbench_selection_service_submit(
        service->selections,
        selection,
        descriptor->default_source_id,
        source_kind,
        trigger);
    if (status != UMI_STATUS_OK) {
        ++service->metrics.error_count;
        ++service->metrics.rejection_count;
        ++service->metrics.revision;
        (void)umi_workbench_selection_provider_history_append(
            &service->history,
            provider_id,
            selection,
            UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_ERROR,
            ++service->sequence,
            selection->timestamp_ms);
        return status;
    }

    if (service->selections->metrics.duplicate_count >
        before_duplicates) {
        ++service->metrics.duplicate_count;
    } else {
        ++service->metrics.selection_publish_count;
    }
    ++service->metrics.revision;
    ++service->revision;

    (void)umi_workbench_selection_provider_history_append(
        &service->history,
        provider_id,
        selection,
        UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_RESOLVED,
        ++service->sequence,
        selection->timestamp_ms);
    return UMI_STATUS_OK;
}

void umi_workbench_selection_provider_service_set_suspended(
    UmiWorkbenchSelectionProviderService *service,
    bool suspended)
{
    if (service == NULL) return;
    service->suspended = suspended;
    ++service->revision;
}

UmiStatus umi_workbench_selection_provider_pipeline_create(
    UmiWorkbenchContextHost *host,
    UmiWorkbenchSelectionProviderPipeline **out_pipeline)
{
    UmiWorkbenchSelectionProviderPipeline *pipeline;
    UmiStatus status;

    if (host == NULL || out_pipeline == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_pipeline = NULL;
    pipeline = (UmiWorkbenchSelectionProviderPipeline *)calloc(
        1U, sizeof(*pipeline));
    if (pipeline == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    /*
     * Construct from the routing boundary upward.  Each layer receives only
     * the immediately lower Framework service that it needs.
     */
    status = umi_workbench_context_event_service_create(
        host, &pipeline->events);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_context_source_service_create(
            pipeline->events, &pipeline->sources);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_selection_service_create(
            pipeline->sources, &pipeline->selections);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_selection_provider_service_create(
            pipeline->selections, &pipeline->providers);
    }

    if (status != UMI_STATUS_OK) {
        umi_workbench_selection_provider_pipeline_destroy(pipeline);
        return status;
    }

    *out_pipeline = pipeline;
    return UMI_STATUS_OK;
}

void umi_workbench_selection_provider_pipeline_destroy(
    UmiWorkbenchSelectionProviderPipeline *pipeline)
{
    if (pipeline == NULL) return;

    /*
     * Reverse-order destruction prevents a higher service from retaining a
     * pointer to a lower service that has already been released.
     */
    umi_workbench_selection_provider_service_destroy(pipeline->providers);
    pipeline->providers = NULL;

    umi_workbench_selection_service_destroy(pipeline->selections);
    pipeline->selections = NULL;

    umi_workbench_context_source_service_destroy(pipeline->sources);
    pipeline->sources = NULL;

    umi_workbench_context_event_service_destroy(pipeline->events);
    pipeline->events = NULL;

    free(pipeline);
}

UmiWorkbenchContextEventService *
umi_workbench_selection_provider_pipeline_event_service(
    UmiWorkbenchSelectionProviderPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->events : NULL;
}

UmiWorkbenchContextSourceService *
umi_workbench_selection_provider_pipeline_source_service(
    UmiWorkbenchSelectionProviderPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->sources : NULL;
}

UmiWorkbenchSelectionService *
umi_workbench_selection_provider_pipeline_selection_service(
    UmiWorkbenchSelectionProviderPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->selections : NULL;
}

UmiWorkbenchSelectionProviderService *
umi_workbench_selection_provider_pipeline_provider_service(
    UmiWorkbenchSelectionProviderPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->providers : NULL;
}

void umi_workbench_selection_provider_pipeline_set_suspended(
    UmiWorkbenchSelectionProviderPipeline *pipeline,
    bool suspended)
{
    if (pipeline == NULL) return;

    /*
     * Suspend every stage.  This makes shutdown, workspace transitions and
     * temporary product-level pauses deterministic rather than allowing an
     * event already accepted by one layer to leak through another layer.
     */
    umi_workbench_selection_provider_service_set_suspended(
        pipeline->providers, suspended);
    umi_workbench_selection_service_set_suspended(
        pipeline->selections, suspended);
    umi_workbench_context_source_service_set_suspended(
        pipeline->sources, suspended);
    umi_workbench_context_event_service_set_suspended(
        pipeline->events, suspended);
}
