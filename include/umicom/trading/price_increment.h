/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/price_increment.h
 * PURPOSE: Review piecewise price increments using exact decimals shared by trading frontends.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_PRICE_INCREMENT_H
#define UMICOM_TRADING_PRICE_INCREMENT_H
#include "umicom/finance/decimal.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_TRADING_PRICE_INCREMENT_LIMIT 64U
    typedef struct UmiTradingPriceIncrement
    {
        UmiDecimal lowerBound;
        UmiDecimal increment;
    } UmiTradingPriceIncrement;
    typedef struct UmiTradingPriceIncrementReview
    {
        size_t bandIndex;
        UmiDecimal increment;
        bool aligned;
    } UmiTradingPriceIncrementReview;
    /* Bands are borrowed for the call. Lower bounds must increase strictly, be
 * nonnegative, and align with their own positive increment. This API supports
 * a grid measured from zero; unsupported shifted grids are refused. */
    UmiStatus UmiTradingPriceIncrementsValidate(const UmiTradingPriceIncrement *bands, size_t count);
    /* Select the greatest lower bound at or below price and check its increment.
 * Below the first band is UNAVAILABLE. Failure preserves out. A successful
 * mathematical check is not broker permission, a lot-size check or an order. */
    UmiStatus UmiTradingPriceIncrementReviewAt(const UmiTradingPriceIncrement *bands, size_t count,
                                               UmiDecimal price, UmiTradingPriceIncrementReview *out);
#ifdef __cplusplus
}
#endif
#endif
