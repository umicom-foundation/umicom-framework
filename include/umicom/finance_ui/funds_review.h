/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance_ui/funds_review.h
 * PURPOSE:
 *   Calculate read-only account and cash forecasts using canonical money arithmetic.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_UI_FUNDS_REVIEW_H
#define UMICOM_FINANCE_UI_FUNDS_REVIEW_H
#include "umicom/finance/money.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Results from values explicitly supplied by the caller, not a bank balance
 * feed. Inputs and outputs use one currency and one caller-selected scale.
 * reserved is nonnegative; a negative ledger/available is shown, not concealed.
 * remaining_after_payment may be negative: this is an analysis, not permission
 * to pay or an overdraft approval. No ledger, account or payment is modified. */
typedef struct UmiFinanceAccountReview {
    UmiMoney available;
    UmiMoney remaining_after_payment;
} UmiFinanceAccountReview;

/** Compare ledger - reserved - proposed_payment. The last two inputs must be
 * nonnegative. All four pointers are required; out changes only on success.
 * See examples/workflow_tools/main.c: GBP 1000 - 100 - 250 = 650. */
UmiStatus UmiFinanceReviewAccount(const UmiMoney *ledger,
    const UmiMoney *reserved, const UmiMoney *proposed_payment,
    UmiFinanceAccountReview *out);

/** Cash forecast from manual or provider-supplied values. Projected cash and
 * buffer headroom can be negative. This creates no trade or settlement request. */
typedef struct UmiFinanceCashReview {
    UmiMoney net_flow;
    UmiMoney projected;
    UmiMoney buffer_headroom;
} UmiFinanceCashReview;

/** Calculate inflows - outflows, then opening + net_flow, then subtract buffer.
 * Nonnegative inflows/outflows/buffer; signed opening. Every operation uses
 * umi_money_add/subtract and checks overflow before publishing the result.
 * A mathematically finite final amount with an unrepresentable intermediate
 * net flow is rejected. Calls are pure and require no services or allocations. */
UmiStatus UmiFinanceReviewCash(const UmiMoney *opening,
    const UmiMoney *inflows, const UmiMoney *outflows, const UmiMoney *buffer,
    UmiFinanceCashReview *out);
#ifdef __cplusplus
}
#endif
#endif
