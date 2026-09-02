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

/**
 * Represent the workbench selection service metrics data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionServiceMetrics {
    uint64_t submitted_count;
    uint64_t resolved_count;
    uint64_t published_count;
    uint64_t duplicate_count;
    uint64_t rejected_count;
    uint64_t revision;
} UmiWorkbenchSelectionServiceMetrics;

/**
 * Represent the workbench selection service data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchSelectionService {
    UmiWorkbenchContextSourceService *sources;
    UmiWorkbenchSelectionServiceMetrics metrics;
    uint64_t sequence;
    uint64_t last_hash;
    uint64_t revision;
    bool suspended;
} UmiWorkbenchSelectionService;

/**
 * Initialise workbench selection service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_selection_service_create(
    UmiWorkbenchContextSourceService *sources,
    UmiWorkbenchSelectionService **out_service);
/**
 * Release or reset state held by workbench selection service so the same storage can be
 * reused safely.
 */
void umi_workbench_selection_service_destroy(
    UmiWorkbenchSelectionService *service);
/**
 * Provide the workbench selection service submit operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_service_submit(
    UmiWorkbenchSelectionService *service,
    UmiWorkbenchSelection *selection,
    const char *source_id,
    UmiWorkbenchContextSourceKind source_kind,
    UmiWorkbenchContextSourceTrigger trigger);
/**
 * Provide the workbench selection service set suspended operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_service_set_suspended(
    UmiWorkbenchSelectionService *service,
    bool suspended);

#ifdef __cplusplus
}
#endif
#endif
