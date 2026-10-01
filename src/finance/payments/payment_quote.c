/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_quote.c
 * PURPOSE: Own one immutable quote and compose existing checked money contracts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/payments/payment_quote.h"
#include "umicom/finance/identifier.h"
#include "umicom/finance/currency.h"
#include <stdlib.h>

struct UmiPaymentQuote { UmiPaymentQuoteSnapshot snapshot; };
UmiStatus UmiPaymentQuoteCreate(const UmiPaymentQuoteRequest *request, UmiPaymentQuote **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (request == NULL || !umi_financial_id_valid(&request->quoteId) || !umi_financial_id_valid(&request->paymentId) ||
        !umi_currency_valid(&request->principal.currency) || request->principal.scale > 9U || request->principal.minor_units <= 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiPaymentQuoteSnapshot s = {0};
    s.request = *request; s.fee = s.tax = s.charges = s.totalDebit = request->principal;
    UmiStatus status = umi_payments_payment_fee_rule_calculate(&request->rule, request->principal.minor_units,
        request->feeRounding, &s.fee.minor_units);
    if (status == UMI_STATUS_OK) status = UmiMoneyApplyBasisPoints(&s.fee, request->taxBasisPoints, request->taxRounding, &s.tax);
    /* Keep fee and tax components explicit. A cap applies to the fee before tax,
     * so tax can make total charges exceed the fee cap without changing it. */
    UmiPaymentsPaymentCharge charge;
    if (status == UMI_STATUS_OK) status = umi_payments_payment_charge_init(&charge, request->quoteId.value,
        request->paymentId.value, s.fee.minor_units, s.tax.minor_units);
    if (status == UMI_STATUS_OK) status = umi_payments_payment_charge_total_checked(&charge, &s.charges.minor_units);
    if (status == UMI_STATUS_OK) status = umi_money_add(&request->principal, &s.charges, &s.totalDebit);
    if (status != UMI_STATUS_OK) return status;
    UmiPaymentQuote *quote = malloc(sizeof(*quote));
    if (quote == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    quote->snapshot = s; *out = quote;
    return UMI_STATUS_OK;
}
void UmiPaymentQuoteDestroy(UmiPaymentQuote *quote) { free(quote); }
UmiStatus UmiPaymentQuoteRead(const UmiPaymentQuote *quote, UmiPaymentQuoteSnapshot *out)
{
    if (quote == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = quote->snapshot;
    return UMI_STATUS_OK;
}
