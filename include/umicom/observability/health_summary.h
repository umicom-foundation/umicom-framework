/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/health_summary.h
 *
 * PURPOSE:
 *   Publish the public health summary contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OBSERVABILITY_HEALTH_SUMMARY_H
#define UMICOM_OBSERVABILITY_HEALTH_SUMMARY_H
#include "umicom/observability/operations_contract.h"
#include "umicom/runtime/health.h"
#include "umicom/diagnostics/readiness.h"
typedef struct UmiOperationsHealthSummary { size_t components; size_t ready_components; size_t degraded_components; size_t failed_components; size_t readiness_checks; size_t failed_readiness_checks; UmiOperationsHealth overall; } UmiOperationsHealthSummary;
UmiStatus umi_operations_health_summarise(const UmiHealthRegistry *health,const UmiReadinessRegistry *readiness,UmiOperationsHealthSummary *out_summary);
#endif
