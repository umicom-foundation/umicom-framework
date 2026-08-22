/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/snapshot.h
 *
 * PURPOSE:
 *   Project immutable live-source inventory and throughput state for diagnostics and frontend adapters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SNAPSHOT_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SNAPSHOT_H
#include "umicom/workbench_context_source/service.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceSnapshot {
    uint32_t structure_size;
    size_t source_count;
    size_t active_source_count;
    size_t runtime_count;
    UmiWorkbenchContextSourceServiceMetrics metrics;
    uint64_t queued_event_count;
    uint64_t revision;
    bool suspended;
} UmiWorkbenchContextSourceSnapshot;

UmiStatus umi_workbench_context_source_snapshot_build(
    const UmiWorkbenchContextSourceService *service,
    UmiWorkbenchContextSourceSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
