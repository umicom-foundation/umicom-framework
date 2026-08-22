/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/service.h
 *
 * PURPOSE:
 *   Own live interaction source registration, per-source deduplication/throttling and event-service submission.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SERVICE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SERVICE_H

#include "umicom/workbench_context_source/registry.h"
#include "umicom/workbench_context_source/translator.h"
#include "umicom/workbench_context_event/service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceRuntime {
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    uint64_t last_content_hash;
    uint64_t last_accepted_ms;
    uint64_t accepted_count;
    uint64_t duplicate_count;
    uint64_t throttled_count;
    uint64_t rejected_count;
    uint64_t revision;
} UmiWorkbenchContextSourceRuntime;

typedef struct UmiWorkbenchContextSourceServiceMetrics {
    uint64_t registered_source_count;
    uint64_t submitted_sample_count;
    uint64_t accepted_sample_count;
    uint64_t duplicate_sample_count;
    uint64_t throttled_sample_count;
    uint64_t rejected_sample_count;
    uint64_t translated_event_count;
    uint64_t routed_event_count;
    uint64_t failed_event_count;
    uint64_t revision;
} UmiWorkbenchContextSourceServiceMetrics;

typedef struct UmiWorkbenchContextSourceService {
    UmiWorkbenchContextEventService *events;
    UmiWorkbenchContextSourceRegistry registry;
    UmiWorkbenchContextSourceRuntime *runtime;
    size_t runtime_count;
    size_t runtime_capacity;
    UmiWorkbenchContextSourceServiceMetrics metrics;
    uint64_t sequence;
    uint64_t revision;
    bool suspended;
} UmiWorkbenchContextSourceService;

UmiStatus umi_workbench_context_source_service_create(
    UmiWorkbenchContextEventService *events,
    UmiWorkbenchContextSourceService **out_service);
void umi_workbench_context_source_service_destroy(
    UmiWorkbenchContextSourceService *service);
UmiStatus umi_workbench_context_source_service_register(
    UmiWorkbenchContextSourceService *service,
    const UmiWorkbenchContextSourceDefinition *definition);
UmiStatus umi_workbench_context_source_service_unregister(
    UmiWorkbenchContextSourceService *service,
    const char *source_id);
UmiStatus umi_workbench_context_source_service_submit(
    UmiWorkbenchContextSourceService *service,
    UmiWorkbenchContextSourceSample *sample);
const UmiWorkbenchContextSourceDefinition *
umi_workbench_context_source_service_find(
    const UmiWorkbenchContextSourceService *service,
    const char *source_id);
const UmiWorkbenchContextSourceRuntime *
umi_workbench_context_source_service_runtime(
    const UmiWorkbenchContextSourceService *service,
    const char *source_id);
void umi_workbench_context_source_service_set_suspended(
    UmiWorkbenchContextSourceService *service,
    bool suspended);

#ifdef __cplusplus
}
#endif
#endif
