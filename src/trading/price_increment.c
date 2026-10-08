/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/price_increment.c
 * PURPOSE: Select the applicable price band without rounding a limit or assuming one global tick.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading/price_increment.h"
UmiStatus UmiTradingPriceIncrementsValidate(const UmiTradingPriceIncrement *bands, size_t count)
{
    if (bands == NULL || count == 0U || count > UMI_TRADING_PRICE_INCREMENT_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < count; ++index)
    {
        if (bands[index].lowerBound.scale > 9U || bands[index].lowerBound.coefficient < 0 ||
            bands[index].increment.scale > 9U || bands[index].increment.coefficient <= 0)
            return UMI_STATUS_INVALID_ARGUMENT;
        bool aligned;
        UmiStatus status = UmiDecimalIsMultiple(bands[index].lowerBound, bands[index].increment, &aligned);
        if (status != UMI_STATUS_OK || !aligned)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (index != 0U)
        {
            int comparison;
            status = UmiDecimalCompare(bands[index - 1U].lowerBound, bands[index].lowerBound, &comparison);
            if (status != UMI_STATUS_OK || comparison >= 0)
                return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiTradingPriceIncrementReviewAt(const UmiTradingPriceIncrement *bands, size_t count,
                                           UmiDecimal price, UmiTradingPriceIncrementReview *out)
{
    if (out == NULL || price.scale > 9U || price.coefficient < 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiTradingPriceIncrementsValidate(bands, count);
    if (status != UMI_STATUS_OK)
        return status;
    size_t selected = 0U;
    bool found = false;
    for (size_t index = 0U; index < count; ++index)
    {
        int comparison;
        status = UmiDecimalCompare(price, bands[index].lowerBound, &comparison);
        if (status != UMI_STATUS_OK)
            return status;
        if (comparison < 0)
            break;
        selected = index;
        found = true;
    }
    if (!found)
        return UMI_STATUS_UNAVAILABLE;
    UmiTradingPriceIncrementReview review = {0};
    review.bandIndex = selected;
    review.increment = bands[selected].increment;
    status = UmiDecimalIsMultiple(price, review.increment, &review.aligned);
    if (status == UMI_STATUS_OK)
        *out = review;
    return status;
}
