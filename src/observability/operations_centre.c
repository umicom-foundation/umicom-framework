/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/observability/operations_centre.c
 *
 * PURPOSE:
 *   Implement the operations centre behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Operations Centre | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/observability/operations_centre.h"
#include <string.h>

/*
 * Initialise operations centre from caller-provided values so later operations receive a
 * known state.
 */
void umi_operations_centre_init(UmiOperationsCentre *centre) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (centre != NULL) { (void)memset(centre,0,sizeof(*centre)); centre->revision = 1U; } }
/*
 * Provide the operations centre record resource operation used by this module and its
 * client applications.
 */
UmiStatus umi_operations_centre_record_resource(UmiOperationsCentre *centre,const UmiOperationsResourceSample *sample) { UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT; status = umi_operations_resource_history_add(&centre->resources,sample); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (status == UMI_STATUS_OK) centre->revision += 1U; return status; }
/*
 * Provide the operations centre record benchmark operation used by this module and its
 * client applications.
 */
UmiStatus umi_operations_centre_record_benchmark(UmiOperationsCentre *centre,const UmiOperationsBenchmarkRun *run) { UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT; status = umi_operations_benchmark_history_add(&centre->benchmarks,run); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (status == UMI_STATUS_OK) centre->revision += 1U; return status; }
/*
 * Provide the operations centre record crash operation used by this module and its client
 * applications.
 */
UmiStatus umi_operations_centre_record_crash(UmiOperationsCentre *centre,const UmiOperationsCrashEvidence *crash) { UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT; status = umi_operations_crash_registry_add(&centre->crashes,crash); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (status == UMI_STATUS_OK) centre->revision += 1U; return status; }
/*
 * Provide the operations centre update evidence operation used by this module and its
 * client applications.
 */
UmiStatus umi_operations_centre_update_evidence(UmiOperationsCentre *centre,const UmiOperationsEvidenceCounts *evidence) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (centre == NULL || evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT; centre->evidence = *evidence; centre->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the operations centre update health operation used by this module and its client
 * applications.
 */
UmiStatus umi_operations_centre_update_health(UmiOperationsCentre *centre,const UmiOperationsHealthSummary *health) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (centre == NULL || health == NULL) return UMI_STATUS_INVALID_ARGUMENT; centre->health = *health; centre->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the operations centre update resilience operation used by this module and its
 * client applications.
 */
UmiStatus umi_operations_centre_update_resilience(UmiOperationsCentre *centre,const UmiOperationsResilienceTelemetry *resilience) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (centre == NULL || resilience == NULL) return UMI_STATUS_INVALID_ARGUMENT; centre->resilience = *resilience; centre->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the operations centre snapshot operation used by this module and its client
 * applications.
 */
void umi_operations_centre_snapshot(const UmiOperationsCentre *centre,UmiOperationsCentreSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_snapshot == NULL) return;
    (void)memset(out_snapshot,0,sizeof(*out_snapshot));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL) return;
    out_snapshot->resource_samples = centre->resources.count; out_snapshot->benchmarks = centre->benchmarks.count; out_snapshot->crashes = centre->crashes.count; out_snapshot->unrecovered_crashes = umi_operations_crash_registry_unrecovered(&centre->crashes); out_snapshot->evidence = centre->evidence; out_snapshot->health = centre->health; out_snapshot->resilience = centre->resilience; out_snapshot->revision = centre->revision;
}
