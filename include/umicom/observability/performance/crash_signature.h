/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/crash_signature.h
 *
 * PURPOSE:
 *   Represent crash signatures, correlations and clustering evidence for crash signature.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_CRASH_SIGNATURE_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_CRASH_SIGNATURE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceCrashSignature {
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
} UmiPerformanceCrashSignature;

/* Initialise a versioned crash signature record with stable identities. */
UmiStatus umi_performance_crash_signature_init(UmiPerformanceCrashSignature *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_crash_signature_validate(const UmiPerformanceCrashSignature *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_crash_signature_observe(UmiPerformanceCrashSignature *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_crash_signature_same_identity(const UmiPerformanceCrashSignature *left, const UmiPerformanceCrashSignature *right);
/* Domain-specific policy helper for crash signature. */
uint64_t umi_performance_crash_signature_fingerprint(const char *text);

#ifdef __cplusplus
}
#endif
#endif
