/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/workbench_adapter.h
 *
 * PURPOSE:
 *   Project the canonical UI workbench snapshot into a context-host observation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_WORKBENCH_ADAPTER_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_WORKBENCH_ADAPTER_H
#include "umicom/workbench_context_host/observation.h"
#include "umicom/ui/workbench.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_workbench_context_host_observation_from_workbench(
    const UmiUiWorkbench *workbench,
    uint64_t now_ms,
    UmiWorkbenchContextHostObservation *out_observation);
#ifdef __cplusplus
}
#endif
#endif
