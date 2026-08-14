/* Umicom Framework | Operations Centre | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/observability/operations_centre.h"
#include <string.h>

void umi_operations_centre_init(UmiOperationsCentre *centre) { if (centre != NULL) { (void)memset(centre,0,sizeof(*centre)); centre->revision = 1U; } }
UmiStatus umi_operations_centre_record_resource(UmiOperationsCentre *centre,const UmiOperationsResourceSample *sample) { UmiStatus status; if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT; status = umi_operations_resource_history_add(&centre->resources,sample); if (status == UMI_STATUS_OK) centre->revision += 1U; return status; }
UmiStatus umi_operations_centre_record_benchmark(UmiOperationsCentre *centre,const UmiOperationsBenchmarkRun *run) { UmiStatus status; if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT; status = umi_operations_benchmark_history_add(&centre->benchmarks,run); if (status == UMI_STATUS_OK) centre->revision += 1U; return status; }
UmiStatus umi_operations_centre_record_crash(UmiOperationsCentre *centre,const UmiOperationsCrashEvidence *crash) { UmiStatus status; if (centre == NULL) return UMI_STATUS_INVALID_ARGUMENT; status = umi_operations_crash_registry_add(&centre->crashes,crash); if (status == UMI_STATUS_OK) centre->revision += 1U; return status; }
UmiStatus umi_operations_centre_update_evidence(UmiOperationsCentre *centre,const UmiOperationsEvidenceCounts *evidence) { if (centre == NULL || evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT; centre->evidence = *evidence; centre->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_operations_centre_update_health(UmiOperationsCentre *centre,const UmiOperationsHealthSummary *health) { if (centre == NULL || health == NULL) return UMI_STATUS_INVALID_ARGUMENT; centre->health = *health; centre->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_operations_centre_update_resilience(UmiOperationsCentre *centre,const UmiOperationsResilienceTelemetry *resilience) { if (centre == NULL || resilience == NULL) return UMI_STATUS_INVALID_ARGUMENT; centre->resilience = *resilience; centre->revision += 1U; return UMI_STATUS_OK; }
void umi_operations_centre_snapshot(const UmiOperationsCentre *centre,UmiOperationsCentreSnapshot *out_snapshot)
{
    if (out_snapshot == NULL) return;
    (void)memset(out_snapshot,0,sizeof(*out_snapshot));
    if (centre == NULL) return;
    out_snapshot->resource_samples = centre->resources.count; out_snapshot->benchmarks = centre->benchmarks.count; out_snapshot->crashes = centre->crashes.count; out_snapshot->unrecovered_crashes = umi_operations_crash_registry_unrecovered(&centre->crashes); out_snapshot->evidence = centre->evidence; out_snapshot->health = centre->health; out_snapshot->resilience = centre->resilience; out_snapshot->revision = centre->revision;
}
