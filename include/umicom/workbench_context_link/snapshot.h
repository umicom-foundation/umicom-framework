/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/snapshot.h
 *
 * PURPOSE:
 *   Project immutable workbench context-link state for GTK4 and other frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_SNAPSHOT_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_SNAPSHOT_H

#include "umicom/workbench_context_link/service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkSnapshot {
    uint32_t structure_size;
    size_t group_count;
    size_t binding_count;
    size_t active_context_count;
    size_t history_count;
    size_t pin_count;
    UmiWorkbenchContextLinkMetrics metrics;
    bool suspended;
    uint64_t revision;
} UmiWorkbenchContextLinkSnapshot;

UmiStatus umi_workbench_context_link_snapshot_build(
    const UmiWorkbenchContextLinkService *service,
    UmiWorkbenchContextLinkSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
