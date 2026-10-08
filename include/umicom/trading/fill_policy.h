/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/fill_policy.h
 * PURPOSE: Review full-quantity intent and displayed liquidity independently of a broker transport.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_FILL_POLICY_H
#define UMICOM_TRADING_FILL_POLICY_H
#include "umicom/finance/decimal.h"
#include "umicom/trading/types.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum UmiFullQuantityMode
    {
        UMI_FULL_QUANTITY_IMMEDIATE = 0,
        UMI_FULL_QUANTITY_WAIT = 1,
        UMI_FULL_QUANTITY_WATCH_THEN_IMMEDIATE = 2
    } UmiFullQuantityMode;

    /* All amounts use the provider's quoted price/size units for one instrument.
 * For example, a pence quote needs a limit in pence, not pounds. This record
 * does not identify a contract, authenticate an account or grant permission.
 * Provider adapters must bind it to the exact instrument and subscription. */
    typedef struct UmiFullQuantityPolicy
    {
        UmiFullQuantityMode mode;
        UmiSide side;
        UmiTimeInForce waitingTimeInForce;
        UmiDecimal quantity, limitPrice, extraVisibleQuantity;
        uint64_t maximumAgeMilliseconds, maximumSkewMilliseconds;
    } UmiFullQuantityPolicy;

    /* Price and size arrive independently. A recent price cannot make an old size
 * current. The producer supplies local monotonic receipt times and explicitly
 * distinguishes real-time evidence from delayed, frozen or unknown data. */
    typedef struct UmiLiquidityObservation
    {
        UmiDecimal price, visibleQuantity;
        uint64_t priceReceivedAtMilliseconds, sizeReceivedAtMilliseconds;
        bool available, realtime, stale;
    } UmiLiquidityObservation;

    typedef enum UmiLiquidityAssessment
    {
        UMI_LIQUIDITY_UNAVAILABLE = 0,
        UMI_LIQUIDITY_NOT_REALTIME,
        UMI_LIQUIDITY_STALE,
        UMI_LIQUIDITY_CLOCK_MISMATCH,
        UMI_LIQUIDITY_FIELDS_SKEWED,
        UMI_LIQUIDITY_OUTSIDE_LIMIT,
        UMI_LIQUIDITY_INSUFFICIENT,
        UMI_LIQUIDITY_MEETS_DISPLAYED_RULE
    } UmiLiquidityAssessment;

    typedef struct UmiFullQuantityReview
    {
        UmiLiquidityAssessment assessment;
        UmiDecimal requiredVisibleQuantity;
        uint64_t priceAgeMilliseconds, sizeAgeMilliseconds;
        /* This is a local observation only. It never reserves displayed liquidity
     * or proves broker support, execution, commission, margin or final fills. */
        bool displayedRuleMet;
    } UmiFullQuantityReview;

    UmiStatus UmiFullQuantityPolicyValidate(const UmiFullQuantityPolicy *policy);
    /* Successful evaluation includes ordinary waiting/refusal reasons. Invalid
 * records and arithmetic overflow leave out unchanged. No I/O is performed. */
    UmiStatus UmiFullQuantityEvaluate(const UmiFullQuantityPolicy *policy,
                                      const UmiLiquidityObservation *observation, uint64_t nowMilliseconds,
                                      UmiFullQuantityReview *out);
    const char *UmiLiquidityAssessmentText(UmiLiquidityAssessment assessment);

#ifdef __cplusplus
}
#endif
#endif
