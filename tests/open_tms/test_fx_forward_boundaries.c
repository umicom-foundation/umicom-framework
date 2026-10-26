/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_fx_forward_boundaries.c
 *
 * PURPOSE:
 *   Exercise complete forward lifecycle edges, bounded records and status-bearing valuation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/open_tms/fx_forward_book.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static UmiOpenTmsFxForward Trade(void) {
    UmiOpenTmsFxForward value = {0};
    strcpy(value.tradeId, "forward-1"); strcpy(value.accountId, "account-1");
    strcpy(value.counterpartyId, "counterparty-1");
    strcpy(value.baseCurrency, "GBP"); strcpy(value.quoteCurrency, "USD");
    value.baseNotional = 1000000.0; value.forwardRate = 1.25;
    value.marketForwardRate = 1.26; value.version = 1U;
    return value;
}
static void Equal(const UmiOpenTmsFxForwardBook *left, const UmiOpenTmsFxForwardBook *right) {
    CHECK(memcmp(left, right, sizeof *left) == 0);
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiOpenTmsFxForwardBook *book = malloc(sizeof *book), *before = malloc(sizeof *before);
    CHECK(book != NULL && before != NULL);
    umi_open_tms_fx_forward_book_init(book);
    UmiOpenTmsFxForward trade = Trade();
    if (!strcmp(mode, "matrix")) {
        const int allowed[5][5] = {
            {0,1,0,0,1}, {0,0,1,0,1}, {0,0,0,1,1}, {0,0,0,0,0}, {0,0,0,0,0}
        };
        for (int from = 0; from < 5; ++from) {
            for (int to = 0; to < 5; ++to) {
                umi_open_tms_fx_forward_book_init(book);
                trade = Trade(); trade.state = (UmiOpenTmsFxForwardState)from;
                CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
                memcpy(before, book, sizeof *book);
                UmiStatus status = umi_open_tms_fx_forward_book_transition(book, trade.tradeId,
                    (UmiOpenTmsFxForwardState)to, 2U);
                if (allowed[from][to]) {
                    CHECK(status == UMI_STATUS_OK && book->revision == before->revision + 1U);
                    CHECK(book->trades[0].version == 2U && (int)book->trades[0].state == to);
                } else { CHECK(status == UMI_STATUS_INVALID_STATE); Equal(book, before); }
            }
        }
    } else if (!strcmp(mode, "lifecycle")) {
        CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
        CHECK(umi_open_tms_fx_forward_book_transition(book, trade.tradeId, UMI_OPEN_TMS_FX_FORWARD_CONFIRMED, 2U) == UMI_STATUS_OK);
        CHECK(umi_open_tms_fx_forward_book_transition(book, trade.tradeId, UMI_OPEN_TMS_FX_FORWARD_SETTLEMENT_PENDING, 3U) == UMI_STATUS_OK);
        CHECK(umi_open_tms_fx_forward_book_transition(book, trade.tradeId, UMI_OPEN_TMS_FX_FORWARD_SETTLED, 4U) == UMI_STATUS_OK);
        CHECK(book->revision == 5U && book->count == 1U);
    } else if (!strcmp(mode, "full")) {
        for (size_t i = 0U; i < UMI_OPEN_TMS_FX_FORWARD_CAPACITY; ++i) {
            (void)snprintf(trade.tradeId, sizeof trade.tradeId, "forward-%zu", i);
            CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
        }
        strcpy(trade.tradeId, "one-too-many"); memcpy(before, book, sizeof *book);
        CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_CAPACITY_EXCEEDED); Equal(book, before);
    } else if (!strcmp(mode, "stale-version") || !strcmp(mode, "unknown-state") ||
               !strcmp(mode, "revision-limit") || !strcmp(mode, "not-found")) {
        CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
        if (!strcmp(mode, "revision-limit")) book->revision = UINT64_MAX;
        memcpy(before, book, sizeof *book);
        UmiStatus expected = !strcmp(mode, "stale-version") ? UMI_STATUS_INVALID_STATE :
            !strcmp(mode, "unknown-state") ? UMI_STATUS_INVALID_ARGUMENT :
            !strcmp(mode, "not-found") ? UMI_STATUS_NOT_FOUND : UMI_STATUS_CAPACITY_EXCEEDED;
        CHECK(umi_open_tms_fx_forward_book_transition(book, !strcmp(mode, "not-found") ? "unknown" : trade.tradeId,
            !strcmp(mode, "unknown-state") ? (UmiOpenTmsFxForwardState)88 : UMI_OPEN_TMS_FX_FORWARD_CONFIRMED,
            !strcmp(mode, "stale-version") ? 1U : 2U) == expected);
        Equal(book, before);
    } else if (!strcmp(mode, "corrupt-count") || !strcmp(mode, "corrupt-row") || !strcmp(mode, "uninitialised")) {
        CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
        if (!strcmp(mode, "corrupt-count")) book->count = UMI_OPEN_TMS_FX_FORWARD_CAPACITY + 1U;
        else if (!strcmp(mode, "corrupt-row")) memset(book->trades[0].tradeId, 'x', sizeof book->trades[0].tradeId);
        else book->revision = 0U;
        memcpy(before, book, sizeof *book);
        CHECK(umi_open_tms_fx_forward_book_find(book, "unknown") == NULL);
        CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_open_tms_fx_forward_book_transition(book, trade.tradeId, UMI_OPEN_TMS_FX_FORWARD_CONFIRMED, 2U) == UMI_STATUS_INVALID_STATE);
        Equal(book, before);
    } else if (!strncmp(mode, "value-", 6U)) {
        double out = 1234.0;
        UmiStatus expected = UMI_STATUS_OK;
        if (!strcmp(mode, "value-zero")) trade.marketForwardRate = trade.forwardRate;
        else if (!strcmp(mode, "value-negative")) trade.marketForwardRate = 1.24;
        else if (!strcmp(mode, "value-unavailable")) { trade.marketForwardRate = 0.0; expected = UMI_STATUS_UNAVAILABLE; }
        else if (!strcmp(mode, "value-overflow")) { trade.baseNotional = DBL_MAX; trade.marketForwardRate = DBL_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
        else if (!strcmp(mode, "value-nan")) { trade.baseNotional = NAN; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else if (!strcmp(mode, "value-infinity")) { trade.marketForwardRate = INFINITY; expected = UMI_STATUS_INVALID_ARGUMENT; }
        else CHECK(!strcmp(mode, "value-positive"));
        CHECK(umi_open_tms_fx_forward_mark_to_market_checked(&trade, &out) == expected);
        if (expected == UMI_STATUS_OK) {
            double reference = !strcmp(mode, "value-zero") ? 0.0 : !strcmp(mode, "value-negative") ? -10000.0 : 10000.0;
            CHECK(fabs(out - reference) < 0.01);
            CHECK(umi_open_tms_fx_forward_mark_to_market(&trade) == out);
        } else {
            CHECK(out == 1234.0);
            CHECK(umi_open_tms_fx_forward_mark_to_market(&trade) == 0.0);
        }
    } else if (!strcmp(mode, "invalid-api")) {
        double out = 1234.0;
        CHECK(umi_open_tms_fx_forward_mark_to_market_checked(NULL, &out) == UMI_STATUS_INVALID_ARGUMENT && out == 1234.0);
        CHECK(umi_open_tms_fx_forward_mark_to_market_checked(&trade, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_open_tms_fx_forward_mark_to_market(NULL) == 0.0);
        CHECK(umi_open_tms_fx_forward_book_find(NULL, "unknown") == NULL);
        CHECK(umi_open_tms_fx_forward_book_add(NULL, &trade) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_open_tms_fx_forward_book_transition(NULL, "unknown", UMI_OPEN_TMS_FX_FORWARD_CONFIRMED, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        umi_open_tms_fx_forward_book_init(NULL);
    } else {
        if (!strcmp(mode, "add-nan")) trade.baseNotional = NAN;
        else if (!strcmp(mode, "add-infinity")) trade.forwardRate = INFINITY;
        else if (!strcmp(mode, "add-market-nan")) trade.marketForwardRate = NAN;
        else if (!strcmp(mode, "add-market-negative")) trade.marketForwardRate = -1.0;
        else if (!strcmp(mode, "add-notional-zero")) trade.baseNotional = 0.0;
        else if (!strcmp(mode, "add-rate-zero")) trade.forwardRate = 0.0;
        else if (!strcmp(mode, "add-unterminated-id")) memset(trade.tradeId, 'x', sizeof trade.tradeId);
        else if (!strcmp(mode, "add-unterminated-account")) memset(trade.accountId, 'x', sizeof trade.accountId);
        else if (!strcmp(mode, "add-unterminated-party")) memset(trade.counterpartyId, 'x', sizeof trade.counterpartyId);
        else if (!strcmp(mode, "add-empty")) trade.tradeId[0] = '\0';
        else if (!strcmp(mode, "add-control")) strcpy(trade.accountId, "account\n");
        else if (!strcmp(mode, "add-blank")) strcpy(trade.counterpartyId, "   ");
        else if (!strcmp(mode, "add-currency")) strcpy(trade.baseCurrency, "gbp");
        else if (!strcmp(mode, "add-currency-width")) strcpy(trade.baseCurrency, "GBPP");
        else if (!strcmp(mode, "add-same-currency")) strcpy(trade.quoteCurrency, "GBP");
        else if (!strcmp(mode, "add-state")) trade.state = (UmiOpenTmsFxForwardState)-1;
        else if (!strcmp(mode, "add-zero-market")) trade.marketForwardRate = 0.0;
        else if (!strcmp(mode, "add-duplicate")) CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
        else CHECK(0);
        memcpy(before, book, sizeof *book);
        UmiStatus status = umi_open_tms_fx_forward_book_add(book, &trade);
        if (!strcmp(mode, "add-zero-market")) {
            CHECK(status == UMI_STATUS_OK && book->count == 1U);
            CHECK(umi_open_tms_fx_forward_book_find(book, trade.tradeId) != NULL);
        } else {
            CHECK(status == (!strcmp(mode, "add-duplicate") ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_INVALID_ARGUMENT));
            Equal(book, before);
        }
    }
    free(before); free(book);
    return 0;
}
