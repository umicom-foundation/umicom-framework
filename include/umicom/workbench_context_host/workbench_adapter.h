/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/workbench_adapter.h
 *
 * PURPOSE:
 *   Project the canonical UI workbench snapshot into a context-host observation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_WORKBENCH_ADAPTER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_WORKBENCH_ADAPTER_H
#include "umicom/workbench_context_host/observation.h"
#include "umicom/ui/workbench.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the workbench context host observation from workbench operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_observation_from_workbench(
    const UmiUiWorkbench *workbench,
    uint64_t now_ms,
    UmiWorkbenchContextHostObservation *out_observation);
#ifdef __cplusplus
}
#endif
#endif
