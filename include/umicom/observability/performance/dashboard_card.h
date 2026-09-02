/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/dashboard_card.h
 *
 * PURPOSE:
 *   Represent toolkit-neutral operational dashboard composition for dashboard card.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_DASHBOARD_CARD_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_DASHBOARD_CARD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance dashboard card data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceDashboardCard {
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
} UmiPerformanceDashboardCard;

/* Initialise a versioned dashboard card record with stable identities. */
UmiStatus umi_performance_dashboard_card_init(UmiPerformanceDashboardCard *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_dashboard_card_validate(const UmiPerformanceDashboardCard *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_dashboard_card_observe(UmiPerformanceDashboardCard *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_dashboard_card_same_identity(const UmiPerformanceDashboardCard *left, const UmiPerformanceDashboardCard *right);
/* Domain-specific policy helper for dashboard card. */
double umi_performance_dashboard_card_weighted_score(double primary, double secondary, double secondary_weight);

#ifdef __cplusplus
}
#endif
#endif
