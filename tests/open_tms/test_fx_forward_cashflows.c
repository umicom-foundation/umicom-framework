/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_fx_forward_cashflows.c
 *
 * PURPOSE:
 *   Verify exact forward legs, reviewed evidence and cashflow refusal behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/open_tms/fx_forward_cashflows.h"
#include "umicom/finance/core/cashflow_schedule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        exit(1); \
    } \
} while (0)

static void Refuse(const UmiOpenTmsFxForwardBook *book,
    const UmiOpenTmsFxForwardCashflowRequest *request, UmiStatus expected)
{
    UmiOpenTmsFxForwardCashflows result;
    unsigned char preserved[sizeof result];
    memset(&result, 0x55, sizeof result);
    memcpy(preserved, &result, sizeof preserved);
    CHECK(UmiOpenTmsFxForwardCashflowsCapture(book, request, &result) == expected);
    CHECK(memcmp(preserved, &result, sizeof preserved) == 0);
}

int main(void)
{
    UmiOpenTmsFxForwardBook *book = malloc(sizeof *book);
    UmiCashflowSchedule *schedule = malloc(sizeof *schedule);
    CHECK(book != NULL && schedule != NULL);
    umi_open_tms_fx_forward_book_init(book);
    const UmiOpenTmsFxForward trade = {
        .tradeId = "synthetic", .accountId = "practice", .counterpartyId = "fictional",
        .baseCurrency = "GBP", .quoteCurrency = "USD", .baseNotional = 1000.0,
        .forwardRate = 1.25, .maturityMilliseconds = 10U,
        .state = UMI_OPEN_TMS_FX_FORWARD_CONFIRMED, .version = 3U
    };
    CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
    char confirmation[] = "confirmed-terms";
    const UmiOpenTmsFxForwardCashflowRequest request = {
        .tradeId = trade.tradeId, .bookRevision = book->revision, .tradeVersion = trade.version,
        .confirmationId = confirmation, .confirmationRevision = 6U, .paymentDate = {2026, 12, 1},
        .baseNotional = {100000, 2U, {"GBP"}}, .contractRate = {125, 2U}, .quoteScale = 2U,
        .receiveId = "receive-base", .payId = "pay-quote"
    };
    UmiOpenTmsFxForwardCashflows result;
    CHECK(UmiOpenTmsFxForwardCashflowsCapture(book, &request, &result) == UMI_STATUS_OK);
    CHECK(result.bookRevision == book->revision && result.trade.version == trade.version);
    CHECK(result.trade.maturityMilliseconds == trade.maturityMilliseconds && result.trade.marketForwardRate == 0.0);
    CHECK(result.contractRate.coefficient == 125 && result.contractRate.scale == 2U);
    CHECK(result.confirmationRevision == 6U);
    confirmation[0] = 'C';
    CHECK(strcmp(result.confirmationId.value, "confirmed-terms") == 0);
    CHECK(strcmp(result.receive.cashflow_id.value, "receive-base") == 0);
    CHECK(strcmp(result.pay.cashflow_id.value, "pay-quote") == 0);
    CHECK(strcmp(result.receive.trade_id.id.value, "synthetic") == 0);
    CHECK(strcmp(result.pay.trade_id.id.value, "synthetic") == 0);
    CHECK(result.receive.payment_date.year == 2026 && result.receive.payment_date.month == 12U &&
        result.receive.payment_date.day == 1U);
    CHECK(umi_financial_date_compare(result.pay.payment_date, result.receive.payment_date) == 0);
    CHECK(result.receive.direction == UMI_FINANCIAL_DIRECTION_RECEIVE);
    CHECK(result.receive.amount.minor_units == 100000 && result.receive.amount.scale == 2U);
    CHECK(strcmp(result.receive.amount.currency.code, "GBP") == 0);
    CHECK(result.pay.direction == UMI_FINANCIAL_DIRECTION_PAY);
    CHECK(result.pay.amount.minor_units == 125000 && result.pay.amount.scale == 2U);
    CHECK(strcmp(result.pay.amount.currency.code, "USD") == 0);
    umi_cashflow_schedule_init(schedule);
    CHECK(umi_cashflow_schedule_add(schedule, &result.receive) == UMI_STATUS_OK);
    CHECK(umi_cashflow_schedule_add(schedule, &result.pay) == UMI_STATUS_OK);
    CHECK(umi_cashflow_schedule_count(schedule) == 2U);
    free(schedule);

    UmiOpenTmsFxForwardCashflowRequest bad = request;
    bad.bookRevision += 1U;
    Refuse(book, &bad, UMI_STATUS_BUSY);
    bad = request; bad.tradeVersion += 1U;
    Refuse(book, &bad, UMI_STATUS_BUSY);
    bad = request; bad.contractRate.coefficient = 126;
    Refuse(book, &bad, UMI_STATUS_BUSY);
    bad = request; bad.baseNotional.minor_units += 1;
    Refuse(book, &bad, UMI_STATUS_BUSY);
    bad = request; bad.baseNotional.currency = (UmiCurrency){"USD"};
    Refuse(book, &bad, UMI_STATUS_BUSY);
    bad = request; bad.baseNotional.currency = (UmiCurrency){{'G', 'B', 'P', 'X'}};
    Refuse(book, &bad, UMI_STATUS_INVALID_ARGUMENT);
    bad = request; bad.tradeId = "missing";
    Refuse(book, &bad, UMI_STATUS_NOT_FOUND);
    bad = request; bad.confirmationId = "";
    Refuse(book, &bad, UMI_STATUS_INVALID_ARGUMENT);
    bad = request; bad.confirmationRevision = 0U;
    Refuse(book, &bad, UMI_STATUS_INVALID_ARGUMENT);
    bad = request; bad.payId = NULL;
    Refuse(book, &bad, UMI_STATUS_INVALID_ARGUMENT);
    bad = request; bad.payId = request.receiveId;
    Refuse(book, &bad, UMI_STATUS_ALREADY_EXISTS);
    bad = request; bad.paymentDate = (UmiFinancialDate){2026, 2, 29};
    Refuse(book, &bad, UMI_STATUS_INVALID_ARGUMENT);
    bad = request; bad.quoteScale = 10U;
    Refuse(book, &bad, UMI_STATUS_INVALID_ARGUMENT);
    CHECK(book->trades[0].state == UMI_OPEN_TMS_FX_FORWARD_CONFIRMED && book->revision == request.bookRevision);

    book->trades[0].state = UMI_OPEN_TMS_FX_FORWARD_DRAFT;
    Refuse(book, &request, UMI_STATUS_INVALID_STATE);
    book->trades[0].state = UMI_OPEN_TMS_FX_FORWARD_SETTLED;
    Refuse(book, &request, UMI_STATUS_INVALID_STATE);
    book->trades[0].state = UMI_OPEN_TMS_FX_FORWARD_SETTLEMENT_PENDING;
    CHECK(UmiOpenTmsFxForwardCashflowsCapture(book, &request, &result) == UMI_STATUS_OK);
    book->trades[0].baseNotional = 0.01;
    bad = request; bad.baseNotional.minor_units = 1;
    Refuse(book, &bad, UMI_STATUS_CAPACITY_EXCEEDED);
    bad.quoteScale = 4U;
    CHECK(UmiOpenTmsFxForwardCashflowsCapture(book, &bad, &result) == UMI_STATUS_OK);
    CHECK(result.pay.amount.minor_units == 125 && result.pay.amount.scale == 4U);
    book->count = UMI_OPEN_TMS_FX_FORWARD_CAPACITY + 1U;
    Refuse(book, &request, UMI_STATUS_INVALID_STATE);
    free(book);

    char display[384] = "original";
    CHECK(UmiOpenTmsFxForwardCashflowsExample(display, 8U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(strcmp(display, "original") == 0);
    CHECK(UmiOpenTmsFxForwardCashflowsExample(display, sizeof display) == UMI_STATUS_OK);
    CHECK(strstr(display, "Receive: 1000.00 GBP\nPay: 1250.00 USD\n") != NULL);
    return 0;
}
