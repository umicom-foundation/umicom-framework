/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/observer.h
 *
 * PURPOSE:
 *   Track workbench observations and suppress duplicate context publications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_OBSERVER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_OBSERVER_H
#include "umicom/workbench_context_host/observation.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostObserver {
    UmiWorkbenchContextHostObservation last;
    bool has_last;
    uint64_t observation_count;
    uint64_t duplicate_count;
    uint64_t revision;
} UmiWorkbenchContextHostObserver;
void umi_workbench_context_host_observer_init(UmiWorkbenchContextHostObserver *observer);
uint32_t umi_workbench_context_host_observer_push(
    UmiWorkbenchContextHostObserver *observer,
    const UmiWorkbenchContextHostObservation *observation);
#ifdef __cplusplus
}
#endif
#endif
