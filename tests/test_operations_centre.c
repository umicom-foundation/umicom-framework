/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_operations_centre.c
 *
 * PURPOSE:
 *   Implement the test operations centre behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Operations Centre tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "umicom/observability/operations_centre.h"

int main(void)
{
    UmiOperationsCentre centre;
    UmiOperationsCentreSnapshot snapshot;
    UmiOperationsResourceSample resource = {"worker",10U,42.0,4096U,8U,12U,1U};
    UmiOperationsBenchmarkRun benchmark = {"build","project-a","build-7",11U,900U,25.0,true};
    UmiOperationsCrashEvidence crash = {"crash-7","studio","builder","Compiler terminated","reports/7",12U,77U,false};
    UmiOperationsEvidenceCounts evidence = {12U,4U,8U,3U,15U,13U};
    UmiOperationsHealthSummary health = {2U,1U,1U,0U,2U,0U,UMI_OPERATIONS_HEALTH_DEGRADED};
    UmiOperationsResilienceTelemetry resilience = {2U,1U,1U,0U,1U,UMI_CIRCUIT_CLOSED,20U,2U};
    umi_operations_centre_init(&centre);
    assert(umi_operations_centre_record_resource(&centre,&resource) == UMI_STATUS_OK);
    assert(umi_operations_centre_record_benchmark(&centre,&benchmark) == UMI_STATUS_OK);
    assert(umi_operations_centre_record_crash(&centre,&crash) == UMI_STATUS_OK);
    assert(umi_operations_centre_update_evidence(&centre,&evidence) == UMI_STATUS_OK);
    assert(umi_operations_centre_update_health(&centre,&health) == UMI_STATUS_OK);
    assert(umi_operations_centre_update_resilience(&centre,&resilience) == UMI_STATUS_OK);
    umi_operations_centre_snapshot(&centre,&snapshot);
    assert(snapshot.resource_samples == 1U);
    assert(snapshot.benchmarks == 1U);
    assert(snapshot.unrecovered_crashes == 1U);
    assert(snapshot.evidence.metrics == 12U);
    assert(snapshot.health.overall == UMI_OPERATIONS_HEALTH_DEGRADED);
    return 0;
}
