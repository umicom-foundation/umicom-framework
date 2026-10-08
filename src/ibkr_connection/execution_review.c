/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/execution_review.c
 * PURPOSE: Compare observed stock fills with an intended quantity without inferring order finality.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <string.h>
static const char *CorrectionSeparator(const char *id)
{
    const char *last = strrchr(id, '.');
    if (last == NULL || last == id || last[1] == '\0')
        return NULL;
    for (const char *p = last + 1; *p != '\0'; ++p)
        if (*p < '0' || *p > '9')
            return NULL;
    return last;
}
static bool CorrectionFamily(const char *a, const char *b)
{
    const char *left = CorrectionSeparator(a), *right = CorrectionSeparator(b);
    return left != NULL && right != NULL && left - a == right - b && strncmp(a, b, (size_t)(left - a)) == 0;
}
UmiStatus UmiIbkrExecutionsReviewQuantity(const UmiIbkrExecutionSnapshot *s, uint64_t permanent,
                                          uint32_t contract, const char *side, UmiDecimal requested,
                                          UmiIbkrExecutionQuantityReview *out)
{
    if (s == NULL || out == NULL || permanent == 0U || contract == 0U || !UmiIbkrText(side, 8U, false) ||
        (strcmp(side, "BOT") != 0 && strcmp(side, "SLD") != 0) || requested.scale > 9U ||
        requested.coefficient <= 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!s->complete || s->failed || s->stale || s->requestId == 0U)
        return UMI_STATUS_UNAVAILABLE;
    if (s->count > UMI_IBKR_EXECUTION_OBSERVATION_LIMIT || !UmiIbkrText(s->account, sizeof s->account, false))
        return UMI_STATUS_INVALID_STATE;
    UmiIbkrExecutionQuantityReview result = {0};
    /* Validate the whole caller-supplied capture before publishing even a
     * subset. A damaged row cannot be hidden by selecting another order. */
    for (size_t i = 0U; i < s->count; ++i)
    {
        const UmiIbkrExecutionObservation *row = &s->rows[i];
        if (!UmiIbkrExecutionObservationValid(row) || strcmp(row->account, s->account) != 0)
            return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(row->executionId, s->rows[j].executionId) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    for (size_t i = 0U; i < s->count; ++i)
    {
        const UmiIbkrExecutionObservation *row = &s->rows[i];
        if (row->permanentOrderId != permanent || row->contractId != contract || strcmp(row->side, side) != 0)
            continue;
        ++result.executionCount;
        unsigned segments = 1U;
        for (const char *p = row->executionId; *p != '\0'; ++p)
            if (*p == '.')
                ++segments;
        if (strcmp(row->securityType, "STK") != 0 || segments > 4U || row->price.coefficient <= 0)
            result.unsupportedExecutionShape = true;
        for (size_t j = 0U; j < s->count; ++j)
        {
            if (j == i)
                continue;
            if (CorrectionFamily(row->executionId, s->rows[j].executionId))
                result.correctionNeedsReview = true;
        }
    }
    if (result.executionCount == 0U)
        return UMI_STATUS_NOT_FOUND;
    if (result.correctionNeedsReview || result.unsupportedExecutionShape)
    {
        /* Preserve both reports for inspection. Arrival order is not a verified
         * correction order; presenting either sum would invent certainty. */
        *out = result;
        return UMI_STATUS_OK;
    }
    for (size_t i = 0U; i < s->count; ++i)
    {
        const UmiIbkrExecutionObservation *row = &s->rows[i];
        if (row->permanentOrderId != permanent || row->contractId != contract || strcmp(row->side, side) != 0)
            continue;
        UmiStatus status =
            UmiDecimalAddExact(result.observedQuantity, row->quantity, &result.observedQuantity);
        if (status != UMI_STATUS_OK)
            return status;
    }
    int compared = 0;
    UmiStatus status = UmiDecimalCompare(result.observedQuantity, requested, &compared);
    if (status != UMI_STATUS_OK)
        return status;
    result.belowRequested = compared < 0;
    result.equalsRequested = compared == 0;
    result.exceedsRequested = compared > 0;
    *out = result;
    return UMI_STATUS_OK;
}
