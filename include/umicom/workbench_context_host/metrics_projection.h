/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/metrics_projection.h
 *
 * PURPOSE:
 *   Project operational counters and ratios for developer and operations surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_METRICS_PROJECTION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_METRICS_PROJECTION_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostMetricsProjection {
    UmiWorkbenchContextHostMetrics host;
    UmiWorkbenchContextLinkMetrics link;
    double deliveries_per_publication;
    double drop_ratio;
    double duplicate_observation_ratio;
    uint64_t revision;
} UmiWorkbenchContextHostMetricsProjection;
UmiStatus umi_workbench_context_host_metrics_projection_build(
    const UmiWorkbenchContextHost *host,
    UmiWorkbenchContextHostMetricsProjection *out_projection);
#ifdef __cplusplus
}
#endif
#endif
