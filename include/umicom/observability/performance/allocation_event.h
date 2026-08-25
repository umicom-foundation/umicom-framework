/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/allocation_event.h
 *
 * PURPOSE:
 *   Represent memory allocation evidence and allocation aggregation state for allocation event.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_ALLOCATION_EVENT_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_ALLOCATION_EVENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceAllocationEvent {
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
} UmiPerformanceAllocationEvent;

/* Initialise a versioned allocation event record with stable identities. */
UmiStatus umi_performance_allocation_event_init(UmiPerformanceAllocationEvent *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_allocation_event_validate(const UmiPerformanceAllocationEvent *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_allocation_event_observe(UmiPerformanceAllocationEvent *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_allocation_event_same_identity(const UmiPerformanceAllocationEvent *left, const UmiPerformanceAllocationEvent *right);
/* Domain-specific policy helper for allocation event. */
double umi_performance_allocation_event_weighted_score(double primary, double secondary, double secondary_weight);

#ifdef __cplusplus
}
#endif
#endif
