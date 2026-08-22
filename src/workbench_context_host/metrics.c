/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/metrics.c
 *
 * PURPOSE:
 *   Initialise context-host metric counters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/metrics.h"
#include <string.h>
void umi_workbench_context_host_metrics_init(UmiWorkbenchContextHostMetrics *metrics)
{if(metrics){memset(metrics,0,sizeof(*metrics));metrics->revision=1U;}}
