/* Umicom Framework | Operations Centre | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_OBSERVABILITY_OPERATIONS_CENTRE_H
#define UMICOM_OBSERVABILITY_OPERATIONS_CENTRE_H
#include "umicom/observability/resource_history.h"
#include "umicom/observability/benchmark_history.h"
#include "umicom/observability/crash_registry.h"
#include "umicom/observability/health_summary.h"
#include "umicom/observability/resilience_telemetry.h"
typedef struct UmiOperationsEvidenceCounts { size_t metrics; size_t traces; size_t events; size_t audit_records; size_t profile_samples; uint64_t updated_at_ns; } UmiOperationsEvidenceCounts;
typedef struct UmiOperationsCentre { UmiOperationsResourceHistory resources; UmiOperationsBenchmarkHistory benchmarks; UmiOperationsCrashRegistry crashes; UmiOperationsEvidenceCounts evidence; UmiOperationsHealthSummary health; UmiOperationsResilienceTelemetry resilience; uint64_t revision; } UmiOperationsCentre;
typedef struct UmiOperationsCentreSnapshot { size_t resource_samples; size_t benchmarks; size_t crashes; size_t unrecovered_crashes; UmiOperationsEvidenceCounts evidence; UmiOperationsHealthSummary health; UmiOperationsResilienceTelemetry resilience; uint64_t revision; } UmiOperationsCentreSnapshot;
void umi_operations_centre_init(UmiOperationsCentre *centre);
UmiStatus umi_operations_centre_record_resource(UmiOperationsCentre *centre,const UmiOperationsResourceSample *sample);
UmiStatus umi_operations_centre_record_benchmark(UmiOperationsCentre *centre,const UmiOperationsBenchmarkRun *run);
UmiStatus umi_operations_centre_record_crash(UmiOperationsCentre *centre,const UmiOperationsCrashEvidence *crash);
UmiStatus umi_operations_centre_update_evidence(UmiOperationsCentre *centre,const UmiOperationsEvidenceCounts *evidence);
UmiStatus umi_operations_centre_update_health(UmiOperationsCentre *centre,const UmiOperationsHealthSummary *health);
UmiStatus umi_operations_centre_update_resilience(UmiOperationsCentre *centre,const UmiOperationsResilienceTelemetry *resilience);
void umi_operations_centre_snapshot(const UmiOperationsCentre *centre,UmiOperationsCentreSnapshot *out_snapshot);
#endif
