/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/performance_session.h
 *
 * PURPOSE:
 *   Represent performance regression and budget control state for performance session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_PERFORMANCE_SESSION_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_PERFORMANCE_SESSION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformancePerformanceSession {
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
} UmiPerformancePerformanceSession;

/* Initialise a versioned performance session record with stable identities. */
UmiStatus umi_performance_performance_session_init(UmiPerformancePerformanceSession *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_performance_session_validate(const UmiPerformancePerformanceSession *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_performance_session_observe(UmiPerformancePerformanceSession *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_performance_session_same_identity(const UmiPerformancePerformanceSession *left, const UmiPerformancePerformanceSession *right);
/* Domain-specific policy helper for performance session. */
uint64_t umi_performance_performance_session_duration_ns(uint64_t begin_ns, uint64_t end_ns);

#ifdef __cplusplus
}
#endif
#endif
