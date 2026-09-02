/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/snapshot.h
 *
 * PURPOSE:
 *   Project immutable structured-selection provider service state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SNAPSHOT_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SNAPSHOT_H
#include "umicom/workbench_selection_provider/service.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionProviderSnapshot {
    uint32_t structure_size;
    size_t provider_count;
    size_t history_count;
    UmiWorkbenchSelectionProviderMetrics metrics;
    uint64_t registry_revision;
    uint64_t history_revision;
    uint64_t revision;
    bool suspended;
} UmiWorkbenchSelectionProviderSnapshot;

/**
 * Provide the workbench selection provider snapshot build operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_provider_snapshot_build(
    const UmiWorkbenchSelectionProviderService *service,
    UmiWorkbenchSelectionProviderSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
