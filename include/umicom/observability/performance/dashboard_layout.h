/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/dashboard_layout.h
 *
 * PURPOSE:
 *   Represent toolkit-neutral operational dashboard composition for dashboard layout.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_DASHBOARD_LAYOUT_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_DASHBOARD_LAYOUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceDashboardLayout {
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
} UmiPerformanceDashboardLayout;

/* Initialise a versioned dashboard layout record with stable identities. */
UmiStatus umi_performance_dashboard_layout_init(UmiPerformanceDashboardLayout *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_dashboard_layout_validate(const UmiPerformanceDashboardLayout *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_dashboard_layout_observe(UmiPerformanceDashboardLayout *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_dashboard_layout_same_identity(const UmiPerformanceDashboardLayout *left, const UmiPerformanceDashboardLayout *right);
/* Domain-specific policy helper for dashboard layout. */
bool umi_performance_dashboard_layout_capacity_available(size_t count, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
