/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_quote.h
 * PURPOSE: Capture an explicit fee and tax calculation without changing a payment or ledger.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_QUOTE_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_QUOTE_H
#include "umicom/finance/payments/payment_fee_rule.h"
#include "umicom/finance/payments/payment_charge.h"
#include "umicom/base/csv_document.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentQuote UmiPaymentQuote;
/** One explicit scenario. Principal is positive; all fee-rule minor units share
 * its currency/scale (0..9). Tax is a caller-selected 0..10000 basis-point rate
 * on the capped fee only, rounded separately. No jurisdiction, tax eligibility,
 * provider tariff, currency scale or execution permission is inferred. */
typedef struct UmiPaymentQuoteRequest {
    UmiFinancialId quoteId, paymentId;
    UmiMoney principal;
    UmiPaymentsPaymentFeeRule rule;
    UmiMoneyRounding feeRounding;
    uint32_t taxBasisPoints;
    UmiMoneyRounding taxRounding;
} UmiPaymentQuoteRequest;
typedef struct UmiPaymentQuoteSnapshot {
    UmiPaymentQuoteRequest request;
    UmiMoney fee, tax, charges, totalDebit;
} UmiPaymentQuoteSnapshot;
/** Deep-copy the request and publish a quote only when every amount, including
 * principal + fee + tax, is representable. Output is NULL on failure. There are
 * no borrowed pointers, timestamps, network requests, reservations or writes. */
UmiStatus UmiPaymentQuoteCreate(const UmiPaymentQuoteRequest *request, UmiPaymentQuote **out);
void UmiPaymentQuoteDestroy(UmiPaymentQuote *quote);
/** Copy owned evidence. Failure leaves out unchanged. Editing this copy never
 * changes the quote. A caller must create a new quote to use different inputs. */
UmiStatus UmiPaymentQuoteRead(const UmiPaymentQuote *quote, UmiPaymentQuoteSnapshot *out);
#define UMI_PAYMENT_QUOTE_TEXT_CAPACITY 4096U
/** Format a complete review. A NULL/zero buffer queries required bytes including
 * the terminator. Insufficient capacity leaves output unchanged. */
UmiStatus UmiPaymentQuoteDescribe(const UmiPaymentQuote *quote, char *output,
    size_t capacity, size_t *required);
/** Export copied assumptions and exact integer amounts. The CSV document is
 * independently owned and can outlive both request and quote. */
UmiStatus UmiPaymentQuoteExportCsv(const UmiPaymentQuote *quote, UmiCsvDocument **out);
#ifdef __cplusplus
}
#endif
#endif
