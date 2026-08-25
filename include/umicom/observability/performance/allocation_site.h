/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/allocation_site.h
 *
 * PURPOSE:
 *   Represent memory allocation evidence and allocation aggregation state for allocation site.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_ALLOCATION_SITE_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_ALLOCATION_SITE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceAllocationSite {
    uint32_t structure_size;
    uint32_t api_version;
    char id[UMI_PERFORMANCE_ID_CAPACITY];
    char subject_id[UMI_PERFORMANCE_ID_CAPACITY];
    UmiPerformanceState state;
    UmiPerformanceSeverity severity;
    uint64_t sequence;
    uint64_t timestamp_ns;
    double value;
    double auxiliary;
    uint64_t count;
    bool enabled;
} UmiPerformanceAllocationSite;

/* Initialise a versioned allocation site record with stable identities. */
UmiStatus umi_performance_allocation_site_init(UmiPerformanceAllocationSite *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_allocation_site_validate(const UmiPerformanceAllocationSite *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_allocation_site_observe(UmiPerformanceAllocationSite *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_allocation_site_same_identity(const UmiPerformanceAllocationSite *left, const UmiPerformanceAllocationSite *right);
/* Domain-specific policy helper for allocation site. */
double umi_performance_allocation_site_weighted_score(double primary, double secondary, double secondary_weight);

#ifdef __cplusplus
}
#endif
#endif
