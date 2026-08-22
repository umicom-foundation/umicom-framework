/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/service.h
 *
 * PURPOSE:
 *   Own provider registration, structured selection publication, history and operational metrics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SERVICE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SERVICE_H

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

#ifdef __cplusplus
}
#endif
#endif
