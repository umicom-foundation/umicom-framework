/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/service.h
 *
 * PURPOSE:
 *   Own provider registration, structured selection publication, history,
 *   operational metrics and the reusable provider runtime pipeline.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SERVICE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SERVICE_H

#include "umicom/workbench_context_host/host.h"
#include "umicom/workbench_selection_provider/history.h"
#include "umicom/workbench_selection_provider/registry.h"
#include "umicom/workbench_selection/service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderMetrics {
    uint64_t provider_registration_count;
    uint64_t provider_removal_count;
    uint64_t selection_submit_count;
    uint64_t selection_publish_count;
    uint64_t duplicate_count;
    uint64_t rejection_count;
    uint64_t error_count;
    uint64_t revision;
} UmiWorkbenchSelectionProviderMetrics;

typedef struct UmiWorkbenchSelectionProviderService {
    UmiWorkbenchSelectionService *selections;
    UmiWorkbenchSelectionProviderRegistry registry;
    UmiWorkbenchSelectionProviderHistory history;
    UmiWorkbenchSelectionProviderMetrics metrics;
    uint64_t sequence;
    uint64_t revision;
    bool suspended;
} UmiWorkbenchSelectionProviderService;

/*
 * The pipeline is intentionally opaque.  It owns the four Framework services
 * needed to turn a structured model selection into a Context Host publication:
 *
 * model/provider -> selection -> live source -> event -> Context Host
 *
 * The caller owns the Context Host.  The pipeline never destroys or replaces
 * that host, which lets Studio, Trader, TMS and future products compose the
 * same reusable runtime around their own workbench profiles.
 */
typedef struct UmiWorkbenchSelectionProviderPipeline
    UmiWorkbenchSelectionProviderPipeline;

UmiStatus umi_workbench_selection_provider_service_create(
    UmiWorkbenchSelectionService *selections,
    UmiWorkbenchSelectionProviderService **out_service);
void umi_workbench_selection_provider_service_destroy(
    UmiWorkbenchSelectionProviderService *service);
UmiStatus umi_workbench_selection_provider_service_register(
    UmiWorkbenchSelectionProviderService *service,
    const UmiWorkbenchSelectionProviderDescriptor *descriptor);
UmiStatus umi_workbench_selection_provider_service_unregister(
    UmiWorkbenchSelectionProviderService *service,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_service_publish(
    UmiWorkbenchSelectionProviderService *service,
    const char *provider_id,
    UmiWorkbenchSelection *selection,
    UmiWorkbenchContextSourceKind source_kind,
    UmiWorkbenchContextSourceTrigger trigger);
void umi_workbench_selection_provider_service_set_suspended(
    UmiWorkbenchSelectionProviderService *service,
    bool suspended);

/*
 * Build the complete reusable publication pipeline around an existing Context
 * Host.  Creation is transactional: on any failure every already-created
 * service is released before the error is returned.
 */
UmiStatus umi_workbench_selection_provider_pipeline_create(
    UmiWorkbenchContextHost *host,
    UmiWorkbenchSelectionProviderPipeline **out_pipeline);
void umi_workbench_selection_provider_pipeline_destroy(
    UmiWorkbenchSelectionProviderPipeline *pipeline);

/* Accessors expose Framework services without transferring ownership. */
UmiWorkbenchContextEventService *
umi_workbench_selection_provider_pipeline_event_service(
    UmiWorkbenchSelectionProviderPipeline *pipeline);
UmiWorkbenchContextSourceService *
umi_workbench_selection_provider_pipeline_source_service(
    UmiWorkbenchSelectionProviderPipeline *pipeline);
UmiWorkbenchSelectionService *
umi_workbench_selection_provider_pipeline_selection_service(
    UmiWorkbenchSelectionProviderPipeline *pipeline);
UmiWorkbenchSelectionProviderService *
umi_workbench_selection_provider_pipeline_provider_service(
    UmiWorkbenchSelectionProviderPipeline *pipeline);

/*
 * Suspend or resume the complete chain as one unit.  Suspension is propagated
 * from the provider boundary down to the event boundary so no partial
 * publication can continue while the application is paused or shutting down.
 */
void umi_workbench_selection_provider_pipeline_set_suspended(
    UmiWorkbenchSelectionProviderPipeline *pipeline,
    bool suspended);

#ifdef __cplusplus
}
#endif
#endif
