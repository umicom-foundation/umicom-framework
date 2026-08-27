/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_timeline_event.h
 *
 * PURPOSE:
 *   Represent one timestamped debugger lifecycle, stop or navigation event.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_TIMELINE_EVENT_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_TIMELINE_EVENT_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugTimelineEvent {
    UmiDebugWorkbenchEntry value;
    uint64_t sequence;
    uint64_t timestamp_ns;
    UmiDebugWorkbenchSessionPhase phase;
    uint64_t revision;
} UmiDebugWorkbenchDebugTimelineEvent;

UmiStatus umi_debug_workbench_debug_timeline_event_init(UmiDebugWorkbenchDebugTimelineEvent *model, const char *id, uint64_t sequence, uint64_t timestamp_ns, UmiDebugWorkbenchSessionPhase phase, const char *label);
UmiStatus umi_debug_workbench_debug_timeline_event_retime(UmiDebugWorkbenchDebugTimelineEvent *model, uint64_t timestamp_ns);
int umi_debug_workbench_debug_timeline_event_valid(const UmiDebugWorkbenchDebugTimelineEvent *model);

#ifdef __cplusplus
}
#endif
#endif
