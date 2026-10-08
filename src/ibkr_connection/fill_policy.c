/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/fill_policy.c
 * PURPOSE: Map full-quantity intent without weakening it or treating displayed liquidity as execution authority.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/fill_policy.h"
#include <limits.h>
#include <string.h>

UmiStatus UmiIbkrDescribeFillInstruction(const UmiFullQuantityPolicy *policy, UmiIbkrFillInstruction *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiFullQuantityPolicyValidate(policy);
    if (status != UMI_STATUS_OK)
        return status;
    UmiIbkrFillInstruction result = {0};
    memcpy(result.orderType, "LMT", 4U);
    if (policy->mode == UMI_FULL_QUANTITY_WAIT)
    {
        result.allOrNone = true;
        memcpy(result.timeInForce, policy->waitingTimeInForce == UMI_TIF_GTC ? "GTC" : "DAY", 4U);
    }
    else
    {
        memcpy(result.timeInForce, "FOK", 4U);
        result.waitForDisplayedLiquidity = policy->mode == UMI_FULL_QUANTITY_WATCH_THEN_IMMEDIATE;
    }
    *out = result;
    return UMI_STATUS_OK;
}

static bool FillContractValid(const UmiIbkrQuoteContract *contract)
{
    if (contract == NULL || contract->contractId == 0U || contract->contractId > INT_MAX ||
        contract->exchange[0] == '\0' || memchr(contract->exchange, '\0', sizeof contract->exchange) == NULL)
        return false;
    for (const unsigned char *p = (const unsigned char *)contract->exchange; *p != 0U; ++p)
        if (*p < 33U || *p > 126U)
            return false;
    return true;
}

/* Detect decimal scale instead of forcing every value to nine places. A large
 * whole size must not overflow simply because fractional support is available.
 * No exponent, separator or whitespace normalization can alter a provider value. */
static UmiStatus FillQuoteDecimal(const char *text, size_t capacity, UmiDecimal *out)
{
    const char *end = memchr(text, '\0', capacity);
    if (end == NULL || end == text)
        return UMI_STATUS_INVALID_ARGUMENT;
    bool point = false;
    uint8_t scale = 0U;
    for (const char *p = text; p < end; ++p)
    {
        if (*p == '.' && !point)
        {
            point = true;
            continue;
        }
        if (*p < '0' || *p > '9')
            return UMI_STATUS_INVALID_ARGUMENT;
        if (point)
        {
            if (scale == 9U)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            ++scale;
        }
    }
    return UmiDecimalParse(text, (size_t)(end - text), scale, out);
}

UmiStatus UmiIbkrFillObservation(UmiSide side, const UmiIbkrQuoteContract *expectedContract,
                                 uint32_t expectedRequestId, const UmiIbkrQuoteSnapshot *quote,
                                 UmiLiquidityObservation *out)
{
    if ((side != UMI_SIDE_BUY && side != UMI_SIDE_SELL) || quote == NULL || out == NULL ||
        expectedRequestId == 0U || expectedRequestId > INT_MAX || !FillContractValid(expectedContract) ||
        !FillContractValid(&quote->contract))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (quote->requestId != expectedRequestId || quote->contract.contractId != expectedContract->contractId ||
        strcmp(quote->contract.exchange, expectedContract->exchange) != 0)
        return UMI_STATUS_INVALID_STATE;
    const UmiIbkrQuoteValue *price = side == UMI_SIDE_BUY ? &quote->ask : &quote->bid;
    const UmiIbkrQuoteValue *size = side == UMI_SIDE_BUY ? &quote->askSize : &quote->bidSize;
    UmiLiquidityObservation observation = {0};
    observation.available = quote->subscribed && !quote->failed && price->received && size->received &&
                            !price->unavailable && !size->unavailable;
    if (observation.available)
    {
        /* A field received before this subscription belongs to older evidence. */
        if (price->receivedAtMilliseconds < quote->requestedAtMilliseconds ||
            size->receivedAtMilliseconds < quote->requestedAtMilliseconds)
            return UMI_STATUS_INVALID_STATE;
        UmiStatus status = FillQuoteDecimal(price->text, sizeof price->text, &observation.price);
        if (status != UMI_STATUS_OK)
            return status;
        status = FillQuoteDecimal(size->text, sizeof size->text, &observation.visibleQuantity);
        if (status != UMI_STATUS_OK)
            return status;
        observation.realtime = quote->dataType == UMI_IBKR_DATA_REALTIME &&
                               price->dataType == UMI_IBKR_DATA_REALTIME &&
                               size->dataType == UMI_IBKR_DATA_REALTIME;
        observation.stale = price->stale || size->stale;
        observation.priceReceivedAtMilliseconds = price->receivedAtMilliseconds;
        observation.sizeReceivedAtMilliseconds = size->receivedAtMilliseconds;
    }
    *out = observation;
    return UMI_STATUS_OK;
}

UmiStatus UmiIbkrReviewFullQuantity(const UmiFullQuantityPolicy *policy,
                                    const UmiIbkrQuoteContract *expectedContract, uint32_t expectedRequestId,
                                    const UmiIbkrQuoteSnapshot *quote, uint64_t nowMilliseconds,
                                    UmiFullQuantityReview *out)
{
    UmiStatus status = UmiFullQuantityPolicyValidate(policy);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLiquidityObservation observation;
    status = UmiIbkrFillObservation(policy->side, expectedContract, expectedRequestId, quote, &observation);
    if (status != UMI_STATUS_OK)
        return status;
    return UmiFullQuantityEvaluate(policy, &observation, nowMilliseconds, out);
}
