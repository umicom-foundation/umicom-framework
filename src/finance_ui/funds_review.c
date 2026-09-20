/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_ui/funds_review.c
 * PURPOSE:
 *   Use the existing checked money operations for account and cash-flow reviews.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance_ui/funds_review.h"

UmiStatus UmiFinanceReviewAccount(const UmiMoney *ledger,
    const UmiMoney *reserved, const UmiMoney *proposed_payment,
    UmiFinanceAccountReview *out)
{
    if (ledger == NULL || reserved == NULL || proposed_payment == NULL ||
        out == NULL || ledger->scale > 9U || reserved->minor_units < 0 ||
        proposed_payment->minor_units < 0) return UMI_STATUS_INVALID_ARGUMENT;
    UmiFinanceAccountReview result;
    UmiStatus status = umi_money_subtract(ledger, reserved, &result.available);
    if (status == UMI_STATUS_OK)
        status = umi_money_subtract(&result.available, proposed_payment,
                                   &result.remaining_after_payment);
    if (status == UMI_STATUS_OK) *out = result;
    return status;
}

UmiStatus UmiFinanceReviewCash(const UmiMoney *opening,
    const UmiMoney *inflows, const UmiMoney *outflows, const UmiMoney *buffer,
    UmiFinanceCashReview *out)
{
    if (opening == NULL || inflows == NULL || outflows == NULL ||
        buffer == NULL || out == NULL || opening->scale > 9U ||
        inflows->minor_units < 0 || outflows->minor_units < 0 ||
        buffer->minor_units < 0) return UMI_STATUS_INVALID_ARGUMENT;
    UmiFinanceCashReview result;
    UmiStatus status = umi_money_subtract(inflows, outflows, &result.net_flow);
    if (status == UMI_STATUS_OK)
        status = umi_money_add(opening, &result.net_flow, &result.projected);
    if (status == UMI_STATUS_OK)
        status = umi_money_subtract(&result.projected, buffer, &result.buffer_headroom);
    if (status == UMI_STATUS_OK) *out = result;
    return status;
}
