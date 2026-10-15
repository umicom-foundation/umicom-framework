/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/policy_batch_internal.h
 * PURPOSE: Evaluate bounded groups of observations before publishing any results.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_POLICY_BATCH_INTERNAL_H
#define UMICOM_POLICY_BATCH_INTERNAL_H
#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Policy projections are calculations, not commands. Keep their plural form in
 * Framework so a table refresh cannot publish its first rows and then fail on a
 * later observation. Each scalar evaluator owns its domain rules.
 *
 * Inputs, including borrowed labels, remain untouched until every result is
 * ready. Callers serialize access and retain borrowed input storage for the
 * call. This operation supplies no locks, persistence or execution authority. */
#define UMI_DEFINE_POLICY_BATCH(Function, Scalar, Input, Output) \
UmiStatus Function(const Input *inputs, size_t count, Output *outputs, size_t capacity) \
{ \
    if (count == 0U) return UMI_STATUS_OK; \
    if (inputs == NULL || outputs == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    if (count > capacity || count > 4096U || \
        count > (16U * 1024U * 1024U) / sizeof(Output)) \
        return UMI_STATUS_CAPACITY_EXCEEDED; \
    Output *candidate = malloc(count * sizeof(*candidate)); \
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY; \
    UmiStatus status = UMI_STATUS_OK; \
    for (size_t index = 0U; index < count; ++index) { \
        status = Scalar(&inputs[index], &candidate[index]); \
        if (status != UMI_STATUS_OK) break; \
    } \
    /* Unused output capacity is deliberately preserved for the caller. */ \
    if (status == UMI_STATUS_OK) memcpy(outputs, candidate, count * sizeof(*outputs)); \
    free(candidate); \
    return status; \
}
#endif
