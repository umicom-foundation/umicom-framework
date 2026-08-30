/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/service.h
 *
 * PURPOSE:
 *   Submit structured selections through the existing live-source/event/context pipeline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_SERVICE_H
#define UMICOM_WORKBENCH_SELECTION_SERVICE_H

#include "umicom/workbench_selection/resolver.h"
#include "umicom/workbench_context_source/service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionServiceMetrics {
    uint64_t submitted_count;
    uint64_t resolved_count;
    uint64_t published_count;
    uint64_t duplicate_count;
    uint64_t rejected_count;
    uint64_t revision;
} UmiWorkbenchSelectionServiceMetrics;

typedef struct UmiWorkbenchSelectionService {
    UmiWorkbenchContextSourceService *sources;
    UmiWorkbenchSelectionServiceMetrics metrics;
    uint64_t sequence;
    uint64_t last_hash;
    uint64_t revision;
    bool suspended;
} UmiWorkbenchSelectionService;

UmiStatus umi_workbench_selection_service_create(
    UmiWorkbenchContextSourceService *sources,
    UmiWorkbenchSelectionService **out_service);
void umi_workbench_selection_service_destroy(
    UmiWorkbenchSelectionService *service);
UmiStatus umi_workbench_selection_service_submit(
    UmiWorkbenchSelectionService *service,
    UmiWorkbenchSelection *selection,
    const char *source_id,
    UmiWorkbenchContextSourceKind source_kind,
    UmiWorkbenchContextSourceTrigger trigger);
void umi_workbench_selection_service_set_suspended(
    UmiWorkbenchSelectionService *service,
    bool suspended);

#ifdef __cplusplus
}
#endif
#endif
