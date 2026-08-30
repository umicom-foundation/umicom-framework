/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/throttle.h
 *
 * PURPOSE:
 *   Provide deterministic per-source publication throttling for high-frequency selection surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_THROTTLE_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_THROTTLE_H
#include "umicom/workbench_context_host/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostThrottle {
    uint64_t minimum_interval_ms;
    uint64_t last_accepted_ms;
    uint64_t accepted_count;
    uint64_t throttled_count;
} UmiWorkbenchContextHostThrottle;
void umi_workbench_context_host_throttle_init(
    UmiWorkbenchContextHostThrottle *throttle,uint64_t minimum_interval_ms);
bool umi_workbench_context_host_throttle_accept(
    UmiWorkbenchContextHostThrottle *throttle,uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
