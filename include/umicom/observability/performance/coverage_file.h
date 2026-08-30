/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/coverage_file.h
 *
 * PURPOSE:
 *   Represent code-coverage evidence, baselines and regressions for coverage file.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_COVERAGE_FILE_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_COVERAGE_FILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceCoverageFile {
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
} UmiPerformanceCoverageFile;

/* Initialise a versioned coverage file record with stable identities. */
UmiStatus umi_performance_coverage_file_init(UmiPerformanceCoverageFile *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_coverage_file_validate(const UmiPerformanceCoverageFile *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_coverage_file_observe(UmiPerformanceCoverageFile *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_coverage_file_same_identity(const UmiPerformanceCoverageFile *left, const UmiPerformanceCoverageFile *right);
/* Domain-specific policy helper for coverage file. */
double umi_performance_coverage_file_coverage_percent(uint64_t covered, uint64_t total);

#ifdef __cplusplus
}
#endif
#endif
