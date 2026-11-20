/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/fx_forward_cashflows.c
 *
 * PURPOSE:
 *   Derive precise contractual cashflows from reviewed FX forward terms.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/open_tms/fx_forward_cashflows.h"
#include "umicom/base/ascii_number.h"
#include "umicom/finance/currency.h"
#include "umicom/finance/decimal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus MatchesObserved(UmiDecimal exact, double observed)
{
    char formatted[UMI_DECIMAL_TEXT_MAX];
    UmiStatus status = UmiDecimalFormat(exact, formatted, sizeof formatted);
    double projected = 0.0;
    if (status == UMI_STATUS_OK)
        status = umi_ascii_double_parse(formatted, strlen(formatted), &projected);
    return status == UMI_STATUS_OK && projected != observed ? UMI_STATUS_BUSY : status;
}

UmiStatus UmiOpenTmsFxForwardCashflowsCapture(const UmiOpenTmsFxForwardBook *book,
    const UmiOpenTmsFxForwardCashflowRequest *request, UmiOpenTmsFxForwardCashflows *out)
{
    if (book == NULL || request == NULL || out == NULL || request->tradeId == NULL ||
        request->tradeId[0] == '\0' || request->confirmationRevision == 0U ||
        !umi_financial_date_is_valid(request->paymentDate) || !umi_currency_valid(&request->baseNotional.currency) ||
        request->baseNotional.minor_units <= 0 || request->baseNotional.scale > 9U ||
        request->contractRate.coefficient <= 0 || request->contractRate.scale > 9U || request->quoteScale > 9U)
        return UMI_STATUS_INVALID_ARGUMENT;

    const UmiOpenTmsFxForward *trade = umi_open_tms_fx_forward_book_find(book, request->tradeId);
    if (trade == NULL) {
        const UmiStatus valid = umi_open_tms_fx_forward_book_validate(book);
        return valid == UMI_STATUS_OK ? UMI_STATUS_NOT_FOUND : valid;
    }
    if (book->revision != request->bookRevision || trade->version != request->tradeVersion ||
        strcmp(request->baseNotional.currency.code, trade->baseCurrency) != 0)
        return UMI_STATUS_BUSY;
    if (trade->state != UMI_OPEN_TMS_FX_FORWARD_CONFIRMED &&
        trade->state != UMI_OPEN_TMS_FX_FORWARD_SETTLEMENT_PENDING)
        return UMI_STATUS_INVALID_STATE;

    /* Confirm the supplied exact terms agree with the current legacy book
     * observation. Only the exact values participate in the cashflow math. */
    const UmiDecimal base = {request->baseNotional.minor_units, request->baseNotional.scale};
    UmiStatus status = MatchesObserved(base, trade->baseNotional);
    if (status == UMI_STATUS_OK) status = MatchesObserved(request->contractRate, trade->forwardRate);
    UmiDecimal quote;
    if (status == UMI_STATUS_OK)
        status = UmiDecimalMultiplyExact(base, request->contractRate, request->quoteScale, &quote);
    if (status != UMI_STATUS_OK) return status;

    UmiOpenTmsFxForwardCashflows candidate = {.trade = *trade, .bookRevision = book->revision,
        .confirmationRevision = request->confirmationRevision, .contractRate = request->contractRate};
    UmiMoney pay = {.minor_units = quote.coefficient, .scale = quote.scale};
    memcpy(pay.currency.code, trade->quoteCurrency, sizeof pay.currency.code);
    status = umi_financial_id_assign(&candidate.confirmationId, request->confirmationId);
    if (status == UMI_STATUS_OK && !umi_financial_id_is_valid(&candidate.confirmationId))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
        status = umi_cashflow_init(&candidate.receive, request->receiveId, trade->tradeId,
            request->baseNotional, UMI_FINANCIAL_DIRECTION_RECEIVE, request->paymentDate);
    if (status == UMI_STATUS_OK)
        status = umi_cashflow_init(&candidate.pay, request->payId, trade->tradeId,
            pay, UMI_FINANCIAL_DIRECTION_PAY, request->paymentDate);
    if (status == UMI_STATUS_OK &&
        strcmp(candidate.receive.cashflow_id.value, candidate.pay.cashflow_id.value) == 0)
        status = UMI_STATUS_ALREADY_EXISTS;
    if (status == UMI_STATUS_OK) *out = candidate;
    return status;
}

UmiStatus UmiOpenTmsFxForwardCashflowsExample(char *text, size_t capacity)
{
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiOpenTmsFxForwardBook *book = malloc(sizeof *book);
    if (book == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_open_tms_fx_forward_book_init(book);
    const UmiOpenTmsFxForward trade = {.tradeId = "practice-forward", .accountId = "practice",
        .counterpartyId = "fictional-party", .baseCurrency = "GBP", .quoteCurrency = "USD",
        .baseNotional = 1000.0, .forwardRate = 1.25, .state = UMI_OPEN_TMS_FX_FORWARD_CONFIRMED, .version = 1U};
    UmiStatus status = umi_open_tms_fx_forward_book_add(book, &trade);
    UmiOpenTmsFxForwardCashflows flows;
    if (status == UMI_STATUS_OK) {
        const UmiOpenTmsFxForwardCashflowRequest request = {
            .tradeId = trade.tradeId, .bookRevision = book->revision, .tradeVersion = trade.version,
            .confirmationId = "practice-confirmation", .confirmationRevision = 1U, .paymentDate = {2026, 12, 1},
            .baseNotional = {100000, 2U, {"GBP"}}, .contractRate = {125, 2U}, .quoteScale = 2U,
            .receiveId = "practice-receive", .payId = "practice-pay"};
        status = UmiOpenTmsFxForwardCashflowsCapture(book, &request, &flows);
    }
    free(book);
    if (status != UMI_STATUS_OK) return status;

    char receive[UMI_DECIMAL_TEXT_MAX], pay[UMI_DECIMAL_TEXT_MAX];
    status = UmiDecimalFormat((UmiDecimal){flows.receive.amount.minor_units, flows.receive.amount.scale},
        receive, sizeof receive);
    if (status == UMI_STATUS_OK)
        status = UmiDecimalFormat((UmiDecimal){flows.pay.amount.minor_units, flows.pay.amount.scale}, pay, sizeof pay);
    if (status != UMI_STATUS_OK) return status;
    const UmiFinancialDate date = flows.receive.payment_date;
    char rendered[384];
    const int count = snprintf(rendered, sizeof rendered,
        "FX forward practice forecast\nPayment date: %04d-%02u-%02u\nReceive: %s %s\nPay: %s %s\n",
        (int)date.year, (unsigned)date.month, (unsigned)date.day,
        receive, flows.receive.amount.currency.code, pay, flows.pay.amount.currency.code);
    if (count < 0) return UMI_STATUS_IO_ERROR;
    if ((size_t)count >= sizeof rendered || (size_t)count >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(text, rendered, (size_t)count + 1U);
    return UMI_STATUS_OK;
}
