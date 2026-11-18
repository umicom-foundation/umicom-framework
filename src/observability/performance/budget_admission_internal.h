/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/observability/performance/budget_admission_internal.h
 *
 * PURPOSE:
 *   Centralize finite observation admission, sequence exhaustion and bounded budget identity rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_PERFORMANCE_BUDGET_ADMISSION_INTERNAL_H
#define UMICOM_PERFORMANCE_BUDGET_ADMISSION_INTERNAL_H
#include "umicom/observability/performance/types.h"
#include <math.h>
#include <string.h>

/* Archive encoding already requires finite doubles. Admit that same domain
 * before publishing observations so live records remain persistable. Values
 * may be signed measurements; nonnegative resource policy is separate. */
static inline UmiStatus UmiPerformanceBudgetValuesValidate(double value, double auxiliary)
{
    return isfinite(value) && isfinite(auxiliary) ?
        UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
static inline UmiStatus UmiPerformanceBudgetObservationAdmit(
    uint64_t sequence, double value, double auxiliary)
{
    UmiStatus status=UmiPerformanceBudgetValuesValidate(value,auxiliary);
    if(status!=UMI_STATUS_OK)return status;
    return sequence==UINT64_MAX ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
/* Stable identity ignores measurements and revisions, but it never reads
 * beyond the fixed identity fields in pursuit of a missing terminator. */
static inline bool UmiPerformanceBudgetIdentityMatches(const char *leftId,
    const char *leftSubject, const char *rightId, const char *rightSubject)
{
    return umi_performance_id_valid(leftId) && umi_performance_id_valid(leftSubject) &&
        umi_performance_id_valid(rightId) && umi_performance_id_valid(rightSubject) &&
        strcmp(leftId,rightId)==0 && strcmp(leftSubject,rightSubject)==0;
}
/* A missing/unbounded limit needs an explicit host policy, never infinity
 * disguised as successful finite capacity. This comparison reserves nothing. */
static inline bool UmiPerformanceWithinFiniteBudget(double used, double limit)
{
    return isfinite(used) && isfinite(limit) && used>=0.0 && limit>=0.0 && used<=limit;
}
#endif
