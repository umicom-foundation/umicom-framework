/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/service.c
 *
 * PURPOSE:
 *   Implement live source registration, Event Service authorisation, duplicate/throttle policy and synchronous context routing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/service.h"

#include <stdlib.h>
#include <string.h>

static UmiStatus ensure_runtime_capacity(
    UmiWorkbenchContextSourceService *service)
{
    UmiWorkbenchContextSourceRuntime *runtime;
    size_t next;
    if (service->runtime_count < service->runtime_capacity) {
        return UMI_STATUS_OK;
    }
    if (service->runtime_capacity >= UMI_WORKBENCH_CONTEXT_SOURCE_MAX_SOURCES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    next = service->runtime_capacity == 0U
        ? 16U
        : service->runtime_capacity * 2U;
    if (next > UMI_WORKBENCH_CONTEXT_SOURCE_MAX_SOURCES) {
        next = UMI_WORKBENCH_CONTEXT_SOURCE_MAX_SOURCES;
    }
    runtime = (UmiWorkbenchContextSourceRuntime *)realloc(
        service->runtime,
        next * sizeof(service->runtime[0]));
    if (runtime == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (next > service->runtime_capacity) {
        memset(
            runtime + service->runtime_capacity,
            0,
            (next - service->runtime_capacity) *
                sizeof(runtime[0]));
    }
    service->runtime = runtime;
    service->runtime_capacity = next;
    return UMI_STATUS_OK;
}

static UmiWorkbenchContextSourceRuntime *runtime_find_mutable(
    UmiWorkbenchContextSourceService *service,
    const char *source_id)
{
    size_t index;
    if (service == NULL || source_id == NULL) return NULL;
    for (index = 0U; index < service->runtime_count; ++index) {
        if (strcmp(service->runtime[index].source_id, source_id) == 0) {
            return &service->runtime[index];
        }
    }
    return NULL;
}

static UmiStatus runtime_ensure(
    UmiWorkbenchContextSourceService *service,
    const char *source_id,
    UmiWorkbenchContextSourceRuntime **out_runtime)
{
    UmiWorkbenchContextSourceRuntime *runtime;
    UmiStatus status;
    if (service == NULL || source_id == NULL ||
        out_runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    runtime = runtime_find_mutable(service, source_id);
    if (runtime != NULL) {
        *out_runtime = runtime;
        return UMI_STATUS_OK;
    }

    status = ensure_runtime_capacity(service);
    if (status != UMI_STATUS_OK) return status;
    runtime = &service->runtime[service->runtime_count++];
    memset(runtime, 0, sizeof(*runtime));
    status = umi_workbench_context_source_copy_text(
        runtime->source_id, sizeof(runtime->source_id), source_id);
    if (status != UMI_STATUS_OK) {
        --service->runtime_count;
        return status;
    }
    runtime->revision = 1U;
    *out_runtime = runtime;
    return UMI_STATUS_OK;
}

static UmiStatus register_event_source(
    UmiWorkbenchContextSourceService *service,
    const UmiWorkbenchContextSourceDefinition *definition)
{
    UmiWorkbenchContextEventSourceDescriptor source;
    UmiStatus status;

    umi_workbench_context_event_source_descriptor_init(
        &source,
        definition->source_id);
    status = umi_workbench_context_event_source_descriptor_set_source(
        &source,
        definition->source_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_event_source_descriptor_set_subject(
        &source,
        definition->application_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_event_source_descriptor_set_group(
        &source,
        definition->preferred_group_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_event_source_descriptor_set_label(
        &source,
        definition->display_name);
    if (status != UMI_STATUS_OK) return status;
    source.event_kind =
        UMI_WORKBENCH_CONTEXT_EVENT_GENERIC_SELECTION;
    source.context_kind = definition->context_kind;
    source.priority =
        UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_INTERACTIVE;
    source.state = UMI_WORKBENCH_CONTEXT_EVENT_ACCEPTED;

    return umi_workbench_context_event_service_register_source(
        service->events,
        &source);
}

UmiStatus umi_workbench_context_source_service_create(
    UmiWorkbenchContextEventService *events,
    UmiWorkbenchContextSourceService **out_service)
{
    UmiWorkbenchContextSourceService *service;
    if (events == NULL || out_service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_service = NULL;
    service = (UmiWorkbenchContextSourceService *)calloc(
        1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->events = events;
    umi_workbench_context_source_registry_init(&service->registry);
    service->sequence = 1U;
    service->revision = 1U;
    service->metrics.revision = 1U;
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_workbench_context_source_service_destroy(
    UmiWorkbenchContextSourceService *service)
{
    if (service == NULL) return;
    free(service->runtime);
    umi_workbench_context_source_registry_destroy(&service->registry);
    free(service);
}

UmiStatus umi_workbench_context_source_service_register(
    UmiWorkbenchContextSourceService *service,
    const UmiWorkbenchContextSourceDefinition *definition)
{
    UmiWorkbenchContextSourceRuntime *runtime;
    UmiStatus status;
    if (service == NULL || definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_source_definition_validate(definition);
    if (status != UMI_STATUS_OK) return status;
    status = register_event_source(service, definition);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_source_registry_upsert(
        &service->registry, definition);
    if (status != UMI_STATUS_OK) return status;
    status = runtime_ensure(
        service, definition->source_id, &runtime);
    if (status != UMI_STATUS_OK) return status;
    (void)runtime;
    service->metrics.registered_source_count =
        service->registry.count;
    ++service->metrics.revision;
    ++service->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_source_service_unregister(
    UmiWorkbenchContextSourceService *service,
    const char *source_id)
{
    size_t index;
    UmiStatus status;
    if (service == NULL || source_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_source_registry_remove(
        &service->registry, source_id);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < service->runtime_count; ++index) {
        if (strcmp(service->runtime[index].source_id, source_id) == 0) {
            if (index + 1U < service->runtime_count) {
                memmove(
                    &service->runtime[index],
                    &service->runtime[index + 1U],
                    (service->runtime_count - index - 1U) *
                        sizeof(service->runtime[0]));
            }
            --service->runtime_count;
            memset(
                &service->runtime[service->runtime_count],
                0,
                sizeof(service->runtime[0]));
            break;
        }
    }

    service->metrics.registered_source_count =
        service->registry.count;
    ++service->metrics.revision;
    ++service->revision;
    return UMI_STATUS_OK;
}

static UmiWorkbenchContextSourcePolicyDecision decide(
    const UmiWorkbenchContextSourceDefinition *definition,
    UmiWorkbenchContextSourceRuntime *runtime,
    const UmiWorkbenchContextSourceSample *sample)
{
    if (!definition->enabled) {
        return UMI_WORKBENCH_CONTEXT_SOURCE_POLICY_REJECT;
    }
    if (runtime->accepted_count > 0U &&
        definition->minimum_interval_ms > 0U &&
        sample->timestamp_ms >= runtime->last_accepted_ms &&
        sample->timestamp_ms - runtime->last_accepted_ms <
            definition->minimum_interval_ms) {
        return UMI_WORKBENCH_CONTEXT_SOURCE_POLICY_THROTTLE;
    }
    if (runtime->accepted_count > 0U &&
        sample->content_hash == runtime->last_content_hash &&
        sample->timestamp_ms >= runtime->last_accepted_ms &&
        sample->timestamp_ms - runtime->last_accepted_ms <=
            definition->coalescing_window_ms) {
        return UMI_WORKBENCH_CONTEXT_SOURCE_POLICY_COALESCE;
    }
    return UMI_WORKBENCH_CONTEXT_SOURCE_POLICY_ACCEPT;
}

UmiStatus umi_workbench_context_source_service_submit(
    UmiWorkbenchContextSourceService *service,
    UmiWorkbenchContextSourceSample *sample)
{
    const UmiWorkbenchContextSourceDefinition *definition;
    UmiWorkbenchContextSourceRuntime *runtime;
    UmiWorkbenchContextSourcePolicyDecision decision;
    UmiWorkbenchContextEvent event;
    size_t processed = 0U;
    UmiStatus status;

    if (service == NULL || sample == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    ++service->metrics.submitted_sample_count;

    if (service->suspended) {
        ++service->metrics.rejected_sample_count;
        ++service->metrics.revision;
        return UMI_STATUS_BUSY;
    }

    status = umi_workbench_context_source_sample_validate(sample);
    if (status != UMI_STATUS_OK) {
        ++service->metrics.rejected_sample_count;
        ++service->metrics.revision;
        return status;
    }

    definition = umi_workbench_context_source_registry_find(
        &service->registry, sample->source_id);
    if (definition == NULL) {
        ++service->metrics.rejected_sample_count;
        ++service->metrics.revision;
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (!umi_workbench_context_source_definition_accepts(
            definition, sample)) {
        ++service->metrics.rejected_sample_count;
        ++service->metrics.revision;
        return UMI_STATUS_PERMISSION_DENIED;
    }

    status = runtime_ensure(service, sample->source_id, &runtime);
    if (status != UMI_STATUS_OK) return status;
    sample->sequence = ++service->sequence;
    (void)umi_workbench_context_source_sample_refresh_hash(sample);

    decision = decide(definition, runtime, sample);
    if (decision == UMI_WORKBENCH_CONTEXT_SOURCE_POLICY_COALESCE) {
        ++runtime->duplicate_count;
        ++runtime->revision;
        ++service->metrics.duplicate_sample_count;
        ++service->metrics.revision;
        return UMI_STATUS_OK;
    }
    if (decision == UMI_WORKBENCH_CONTEXT_SOURCE_POLICY_THROTTLE) {
        ++runtime->throttled_count;
        ++runtime->revision;
        ++service->metrics.throttled_sample_count;
        ++service->metrics.revision;
        return UMI_STATUS_OK;
    }
    if (decision == UMI_WORKBENCH_CONTEXT_SOURCE_POLICY_REJECT) {
        ++runtime->rejected_count;
        ++runtime->revision;
        ++service->metrics.rejected_sample_count;
        ++service->metrics.revision;
        return UMI_STATUS_PERMISSION_DENIED;
    }

    status = umi_workbench_context_source_translate(
        definition, sample, &event);
    if (status != UMI_STATUS_OK) {
        ++runtime->rejected_count;
        ++runtime->revision;
        ++service->metrics.failed_event_count;
        ++service->metrics.rejected_sample_count;
        ++service->metrics.revision;
        return status;
    }
    ++service->metrics.translated_event_count;

    status = umi_workbench_context_event_service_submit(
        service->events, &event);
    if (status != UMI_STATUS_OK) {
        ++runtime->rejected_count;
        ++runtime->revision;
        ++service->metrics.failed_event_count;
        ++service->metrics.revision;
        return status;
    }
    status = umi_workbench_context_event_service_pump(
        service->events, 0U, &processed);
    if (status != UMI_STATUS_OK) {
        ++runtime->rejected_count;
        ++runtime->revision;
        ++service->metrics.failed_event_count;
        ++service->metrics.revision;
        return status;
    }

    runtime->last_content_hash = sample->content_hash;
    runtime->last_accepted_ms = sample->timestamp_ms;
    ++runtime->accepted_count;
    ++runtime->revision;
    ++service->metrics.accepted_sample_count;
    service->metrics.routed_event_count += (uint64_t)processed;
    ++service->metrics.revision;
    ++service->revision;
    return UMI_STATUS_OK;
}

const UmiWorkbenchContextSourceDefinition *
umi_workbench_context_source_service_find(
    const UmiWorkbenchContextSourceService *service,
    const char *source_id)
{
    if (service == NULL) return NULL;
    return umi_workbench_context_source_registry_find(
        &service->registry, source_id);
}

const UmiWorkbenchContextSourceRuntime *
umi_workbench_context_source_service_runtime(
    const UmiWorkbenchContextSourceService *service,
    const char *source_id)
{
    size_t index;
    if (service == NULL || source_id == NULL) return NULL;
    for (index = 0U; index < service->runtime_count; ++index) {
        if (strcmp(service->runtime[index].source_id, source_id) == 0) {
            return &service->runtime[index];
        }
    }
    return NULL;
}

void umi_workbench_context_source_service_set_suspended(
    UmiWorkbenchContextSourceService *service,
    bool suspended)
{
    if (service == NULL) return;
    service->suspended = suspended;
    ++service->revision;
}
