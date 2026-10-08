/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/execution_commission.c
 * PURPOSE: Correlate separately delivered commission reports without treating missing or conflicting fees as zero.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus CommissionIgnored(UmiIbkrConnection *c)
{
    if (c->snapshot.ignoredFrames == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    ++c->snapshot.ignoredFrames;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrCommissionFrame(UmiIbkrConnection *c, const unsigned char *body, size_t length, uint64_t now)
{
    UmiIbkrExecutionSnapshot *s = &c->executions;
    if (c->snapshot.state != UMI_IBKR_READY || s->requestId == 0U || s->failed ||
        now < s->requestedAtMilliseconds ||
        now - s->requestedAtMilliseconds >= c->options.timeoutMilliseconds)
        return CommissionIgnored(c);
    if (length == 0U || length > UMI_IBKR_FRAME_LIMIT || body[length - 1U] != 0U)
        return UMI_STATUS_PARSE_ERROR;
    char *copy = malloc(length), *f[8];
    if (copy == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, body, length);
    size_t count = 0U, start = 0U;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < length; ++i)
        if (copy[i] == '\0')
        {
            if (count == 8U)
            {
                status = UMI_STATUS_PARSE_ERROR;
                goto done;
            }
            f[count++] = copy + start;
            start = i + 1U;
        }
    if (count != 8U || strcmp(f[1], "1") != 0 || !UmiIbkrText(f[2], 128U, false))
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    size_t index = 0U;
    while (index < s->count && strcmp(s->rows[index].executionId, f[2]) != 0)
        ++index;
    if (index == s->count)
    {
        status = CommissionIgnored(c);
        goto done;
    }
    if (!UmiIbkrText(f[3], 96U, false) || !UmiIbkrText(f[4], 16U, false) || !UmiIbkrDecimalText(f[3]) ||
        !UmiIbkrDecimalText(f[5]) || !UmiIbkrDecimalText(f[6]))
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    uint64_t date;
    if (!UmiIbkrUnsigned(f[7], &date) || date > UINT32_MAX)
    {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    if (s->revision == UINT64_MAX)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    UmiIbkrExecutionFee candidate = s->fees[index];
    UmiDecimal amount = {0};
    bool exact = UmiDecimalParseScientificExact(f[3], strlen(f[3]), &amount) == UMI_STATUS_OK;
    if (candidate.received)
    {
        int comparison = 0;
        bool equal = candidate.exact && exact &&
                     UmiDecimalCompare(candidate.amount, amount, &comparison) == UMI_STATUS_OK &&
                     comparison == 0;
        if (strcmp(candidate.firstCurrency, f[4]) != 0 ||
            (!equal && strcmp(candidate.firstReportedAmount, f[3]) != 0))
            candidate.conflicting = true;
    }
    else
    {
        candidate.received = true;
        candidate.exact = exact;
        candidate.amount = amount;
        strcpy(candidate.firstReportedAmount, f[3]);
        strcpy(candidate.firstCurrency, f[4]);
    }
    strcpy(candidate.latestReportedAmount, f[3]);
    strcpy(candidate.latestCurrency, f[4]);
    candidate.receivedAtMilliseconds = now;
    s->fees[index] = candidate;
    ++s->revision;
done:
    free(copy);
    return status;
}
UmiStatus UmiIbkrExecutionsReviewCommission(const UmiIbkrExecutionSnapshot *s, uint64_t permanent,
                                            uint32_t contract, const char *side,
                                            UmiIbkrExecutionCommissionReview *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiIbkrExecutionQuantityReview quantities;
    UmiStatus status =
        UmiIbkrExecutionsReviewQuantity(s, permanent, contract, side, (UmiDecimal){1, 0U}, &quantities);
    if (status != UMI_STATUS_OK)
        return status;
    if (quantities.correctionNeedsReview || quantities.unsupportedExecutionShape)
        return UMI_STATUS_UNAVAILABLE;
    UmiIbkrExecutionCommissionReview result = {0};
    for (size_t i = 0U; i < s->count; ++i)
    {
        const UmiIbkrExecutionObservation *row = &s->rows[i];
        if (row->permanentOrderId != permanent || row->contractId != contract || strcmp(row->side, side) != 0)
            continue;
        const UmiIbkrExecutionFee *fee = &s->fees[i];
        if (!fee->received || !fee->exact)
            return UMI_STATUS_UNAVAILABLE;
        if (fee->conflicting || fee->amount.scale > 9U ||
            !UmiIbkrText(fee->firstCurrency, sizeof fee->firstCurrency, false))
            return UMI_STATUS_INVALID_STATE;
        UmiDecimal first, latest;
        int comparison = 0;
        if (!UmiIbkrText(fee->firstReportedAmount, sizeof fee->firstReportedAmount, false) ||
            !UmiIbkrText(fee->latestReportedAmount, sizeof fee->latestReportedAmount, false) ||
            !UmiIbkrText(fee->latestCurrency, sizeof fee->latestCurrency, false) ||
            strcmp(fee->firstCurrency, fee->latestCurrency) != 0 ||
            UmiDecimalParseScientificExact(fee->firstReportedAmount, strlen(fee->firstReportedAmount),
                                           &first) != UMI_STATUS_OK ||
            UmiDecimalParseScientificExact(fee->latestReportedAmount, strlen(fee->latestReportedAmount),
                                           &latest) != UMI_STATUS_OK ||
            UmiDecimalCompare(first, fee->amount, &comparison) != UMI_STATUS_OK || comparison != 0 ||
            UmiDecimalCompare(latest, fee->amount, &comparison) != UMI_STATUS_OK || comparison != 0)
            return UMI_STATUS_INVALID_STATE;
        if (result.executionCount == 0U)
            strcpy(result.currency, fee->firstCurrency);
        else if (strcmp(result.currency, fee->firstCurrency) != 0)
            return UMI_STATUS_NOT_IMPLEMENTED;
        status = UmiDecimalAddExact(result.amount, fee->amount, &result.amount);
        if (status != UMI_STATUS_OK)
            return status;
        ++result.executionCount;
    }
    *out = result;
    return UMI_STATUS_OK;
}
