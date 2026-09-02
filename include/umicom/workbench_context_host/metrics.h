/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/metrics.h
 *
 * PURPOSE:
 *   Collect routing, observation, navigation and delivery metrics for context-linked workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_METRICS_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_METRICS_H
#include "umicom/workbench_context_host/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the workbench context host metrics data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextHostMetrics {
    uint64_t endpoint_registration_count;
    uint64_t endpoint_removal_count;
    uint64_t profile_apply_count;
    uint64_t observation_count;
    uint64_t duplicate_observation_count;
    uint64_t publication_count;
    uint64_t delivery_queued_count;
    uint64_t delivery_consumed_count;
    uint64_t delivery_dropped_count;
    uint64_t navigation_count;
    uint64_t pin_count;
    uint64_t error_count;
    uint64_t revision;
} UmiWorkbenchContextHostMetrics;
/**
 * Initialise workbench context host metrics from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_metrics_init(UmiWorkbenchContextHostMetrics *metrics);
#ifdef __cplusplus
}
#endif
#endif
