/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/metrics_projection.c
 *
 * PURPOSE:
 *   Calculate stable routing and observation ratios from host and link-service counters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/metrics_projection.h"
#include <string.h>
UmiStatus umi_workbench_context_host_metrics_projection_build(
    const UmiWorkbenchContextHost *host,UmiWorkbenchContextHostMetricsProjection *out_projection)
{
    uint64_t attempts;if(!host||!out_projection||!host->link_service)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_projection,0,sizeof(*out_projection));out_projection->host=host->metrics;
    out_projection->link=host->link_service->metrics;
    if(host->metrics.publication_count>0U)out_projection->deliveries_per_publication=
        (double)host->metrics.delivery_queued_count/(double)host->metrics.publication_count;
    attempts=host->metrics.delivery_queued_count+host->metrics.delivery_dropped_count;
    if(attempts>0U)out_projection->drop_ratio=(double)host->metrics.delivery_dropped_count/(double)attempts;
    if(host->metrics.observation_count>0U)out_projection->duplicate_observation_ratio=
        (double)host->metrics.duplicate_observation_count/(double)host->metrics.observation_count;
    out_projection->revision=host->revision;return UMI_STATUS_OK;
}
