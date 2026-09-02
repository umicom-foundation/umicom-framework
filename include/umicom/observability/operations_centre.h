/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/operations_centre.h
 *
 * PURPOSE:
 *   Publish the public operations centre contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OBSERVABILITY_OPERATIONS_CENTRE_H
#define UMICOM_OBSERVABILITY_OPERATIONS_CENTRE_H
#include "umicom/observability/resource_history.h"
#include "umicom/observability/benchmark_history.h"
#include "umicom/observability/crash_registry.h"
#include "umicom/observability/health_summary.h"
#include "umicom/observability/resilience_telemetry.h"
/**
 * Represent the operations evidence counts data shared with callers of this public
 * contract.
 */
typedef struct UmiOperationsEvidenceCounts { size_t metrics; size_t traces; size_t events; size_t audit_records; size_t profile_samples; uint64_t updated_at_ns; } UmiOperationsEvidenceCounts;
/**
 * Represent the operations centre data shared with callers of this public contract.
 */
typedef struct UmiOperationsCentre { UmiOperationsResourceHistory resources; UmiOperationsBenchmarkHistory benchmarks; UmiOperationsCrashRegistry crashes; UmiOperationsEvidenceCounts evidence; UmiOperationsHealthSummary health; UmiOperationsResilienceTelemetry resilience; uint64_t revision; } UmiOperationsCentre;
/**
 * Represent the operations centre snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiOperationsCentreSnapshot { size_t resource_samples; size_t benchmarks; size_t crashes; size_t unrecovered_crashes; UmiOperationsEvidenceCounts evidence; UmiOperationsHealthSummary health; UmiOperationsResilienceTelemetry resilience; uint64_t revision; } UmiOperationsCentreSnapshot;
/**
 * Initialise operations centre from caller-provided values so later operations receive a
 * known state.
 */
void umi_operations_centre_init(UmiOperationsCentre *centre);
/**
 * Provide the operations centre record resource operation used by this module and its
 * client applications.
 */
UmiStatus umi_operations_centre_record_resource(UmiOperationsCentre *centre,const UmiOperationsResourceSample *sample);
/**
 * Provide the operations centre record benchmark operation used by this module and its
 * client applications.
 */
UmiStatus umi_operations_centre_record_benchmark(UmiOperationsCentre *centre,const UmiOperationsBenchmarkRun *run);
/**
 * Provide the operations centre record crash operation used by this module and its client
 * applications.
 */
UmiStatus umi_operations_centre_record_crash(UmiOperationsCentre *centre,const UmiOperationsCrashEvidence *crash);
/**
 * Provide the operations centre update evidence operation used by this module and its
 * client applications.
 */
UmiStatus umi_operations_centre_update_evidence(UmiOperationsCentre *centre,const UmiOperationsEvidenceCounts *evidence);
/**
 * Provide the operations centre update health operation used by this module and its client
 * applications.
 */
UmiStatus umi_operations_centre_update_health(UmiOperationsCentre *centre,const UmiOperationsHealthSummary *health);
/**
 * Provide the operations centre update resilience operation used by this module and its
 * client applications.
 */
UmiStatus umi_operations_centre_update_resilience(UmiOperationsCentre *centre,const UmiOperationsResilienceTelemetry *resilience);
/**
 * Provide the operations centre snapshot operation used by this module and its client
 * applications.
 */
void umi_operations_centre_snapshot(const UmiOperationsCentre *centre,UmiOperationsCentreSnapshot *out_snapshot);
#endif
