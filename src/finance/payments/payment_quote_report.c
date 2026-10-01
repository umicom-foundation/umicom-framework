/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_quote_report.c
 * PURPOSE: Explain exact captured pricing inputs and export independent CSV evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/payments/payment_quote.h"
#include "umicom/finance/decimal.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

UmiStatus UmiPaymentQuoteDescribe(const UmiPaymentQuote *quote, char *output,
    size_t capacity, size_t *required)
{
    if (required != NULL) *required = 0;
    if ((output == NULL && capacity != 0) || (output == NULL && required == NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiPaymentQuoteSnapshot s;
    UmiStatus status = UmiPaymentQuoteRead(quote, &s);
    if (status != UMI_STATUS_OK) return status;
    int64_t values[] = {s.request.principal.minor_units,s.request.rule.fixed_fee_minor,s.request.rule.maximum_fee_minor,
        s.fee.minor_units,s.tax.minor_units,s.charges.minor_units,s.totalDebit.minor_units};
    char amounts[7][UMI_DECIMAL_TEXT_MAX + 1U];
    for (size_t i = 0; i < 7; ++i) {
        status = UmiDecimalFormat((UmiDecimal){.coefficient=values[i],.scale=s.request.principal.scale}, amounts[i], sizeof(amounts[i]));
        if (status != UMI_STATUS_OK) return status;
    }
    char text[UMI_PAYMENT_QUOTE_TEXT_CAPACITY];
    int count = snprintf(text, sizeof(text),
        "PAYMENT FEE QUOTE - CAPTURED SCENARIO\nQuote: %s\nPayment reference: %s\nRule: %s\nCurrency: %s; scale: %u\n"
        "Principal: %s\nFixed fee: %s\nVariable fee: %" PRIu32 " basis points\nFee cap: %s\nFee rounding: %s\n"
        "Fee after cap: %s\nTax on capped fee: %" PRIu32 " basis points\nTax rounding: %s\nTax amount: %s\n"
        "Total charges: %s\nPrincipal plus charges: %s\n\n"
        "One basis point is 0.01 percent. The cap applies to the fee before tax.\n"
        "Inputs are caller-selected assumptions, not a provider tariff or tax determination.\n"
        "No payment was submitted, approved, reserved, posted or sent. A quote is not authorisation.\n",
        s.request.quoteId.value,s.request.paymentId.value,s.request.rule.id.value,s.request.principal.currency.code,(unsigned)s.request.principal.scale,
        amounts[0],amounts[1],s.request.rule.variable_fee_bps,amounts[2],UmiMoneyRoundingName(s.request.feeRounding),amounts[3],
        s.request.taxBasisPoints,UmiMoneyRoundingName(s.request.taxRounding),amounts[4],amounts[5],amounts[6]);
    if (count < 0) return UMI_STATUS_IO_ERROR;
    if ((size_t)count >= sizeof(text)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (required != NULL) *required = (size_t)count + 1U;
    if (output == NULL) return UMI_STATUS_OK;
    if (capacity <= (size_t)count) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(output, text, (size_t)count + 1U);
    return UMI_STATUS_OK;
}
UmiStatus UmiPaymentQuoteExportCsv(const UmiPaymentQuote *quote, UmiCsvDocument **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiPaymentQuoteSnapshot s;
    UmiStatus status = UmiPaymentQuoteRead(quote, &s);
    if (status != UMI_STATUS_OK) return status;
    const char *names[] = {"quote_id","payment_id","rule_id","currency","scale","principal_minor","fixed_fee_minor","variable_fee_bps",
        "maximum_fee_minor","fee_rounding","tax_bps_on_fee","tax_rounding","fee_minor","tax_minor","charges_minor","total_debit_minor","scope"};
    UmiCsvCell headerCells[17];
    for (size_t i = 0; i < 17; ++i) headerCells[i] = UmiCsvText(names[i]);
    UmiCsvCell cells[] = {UmiCsvText(s.request.quoteId.value),UmiCsvText(s.request.paymentId.value),UmiCsvText(s.request.rule.id.value),
        UmiCsvText(s.request.principal.currency.code),UmiCsvUnsigned(s.request.principal.scale),UmiCsvSigned(s.request.principal.minor_units),
        UmiCsvSigned(s.request.rule.fixed_fee_minor),UmiCsvUnsigned(s.request.rule.variable_fee_bps),UmiCsvSigned(s.request.rule.maximum_fee_minor),
        UmiCsvText(UmiMoneyRoundingName(s.request.feeRounding)),UmiCsvUnsigned(s.request.taxBasisPoints),UmiCsvText(UmiMoneyRoundingName(s.request.taxRounding)),
        UmiCsvSigned(s.fee.minor_units),UmiCsvSigned(s.tax.minor_units),UmiCsvSigned(s.charges.minor_units),UmiCsvSigned(s.totalDebit.minor_units),
        UmiCsvText("Caller-selected scenario; no tariff/tax determination, reservation, posting or payment authorisation")};
    UmiCsvDocument *document = NULL;
    status = UmiCsvDocumentCreate(UMI_CSV_MAX_BYTES, &document);
    if (status == UMI_STATUS_OK) status = UmiCsvDocumentAppendRow(document, headerCells, 17);
    if (status == UMI_STATUS_OK) status = UmiCsvDocumentAppendRow(document, cells, 17);
    if (status != UMI_STATUS_OK) { UmiCsvDocumentDestroy(document); return status; }
    *out = document;
    return UMI_STATUS_OK;
}
