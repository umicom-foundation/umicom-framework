/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/leak_candidate.h
 *
 * PURPOSE:
 *   Classify potential memory leaks using bounded evidence for leak candidate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_LEAK_CANDIDATE_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_LEAK_CANDIDATE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance leak candidate data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceLeakCandidate {
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
} UmiPerformanceLeakCandidate;

/* Initialise a versioned leak candidate record with stable identities. */
UmiStatus umi_performance_leak_candidate_init(UmiPerformanceLeakCandidate *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_leak_candidate_validate(const UmiPerformanceLeakCandidate *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_leak_candidate_observe(UmiPerformanceLeakCandidate *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_leak_candidate_same_identity(const UmiPerformanceLeakCandidate *left, const UmiPerformanceLeakCandidate *right);
/* Domain-specific policy helper for leak candidate. */
double umi_performance_leak_candidate_weighted_score(double primary, double secondary, double secondary_weight);

#ifdef __cplusplus
}
#endif
#endif
