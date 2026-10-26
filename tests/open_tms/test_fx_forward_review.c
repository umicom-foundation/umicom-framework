/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_fx_forward_review.c
 *
 * PURPOSE:
 *   Qualify immutable treasury previews, stale-state refusal and complete failure atomicity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/open_tms/fx_forward_review.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static UmiOpenTmsFxForward Trade(const char *id) {
    UmiOpenTmsFxForward trade = {0};
    strcpy(trade.tradeId, id); strcpy(trade.accountId, "treasury-account");
    strcpy(trade.counterpartyId, "synthetic-party");
    strcpy(trade.baseCurrency, "GBP"); strcpy(trade.quoteCurrency, "USD");
    trade.baseNotional = 1000.0; trade.forwardRate = 1.25;
    trade.marketForwardRate = 1.26; trade.maturityMilliseconds = 100U; trade.version = 1U;
    return trade;
}
static UmiOpenTmsFxForwardReview *Prepare(UmiOpenTmsFxForwardBook *book) {
    UmiOpenTmsFxForwardReview *review = NULL;
    CHECK(umi_open_tms_fx_forward_review_create(book, "trade-1",
        UMI_OPEN_TMS_FX_FORWARD_CONFIRMED, 2U, &review) == UMI_STATUS_OK);
    CHECK(review != NULL);
    return review;
}
static void Same(const UmiOpenTmsFxForwardBook *a, const UmiOpenTmsFxForwardBook *b) {
    CHECK(memcmp(a, b, sizeof *a) == 0);
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiOpenTmsFxForwardBook *book = malloc(sizeof *book), *before = malloc(sizeof *before);
    CHECK(book != NULL && before != NULL);
    umi_open_tms_fx_forward_book_init(book);
    UmiOpenTmsFxForward trade = Trade("trade-1");
    CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
    trade = Trade("trade-2");
    CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
    memcpy(before, book, sizeof *book);
    UmiOpenTmsFxForwardReview *review = Prepare(book);
    Same(book, before);
    UmiOpenTmsFxForwardReviewSnapshot snapshot;
    CHECK(umi_open_tms_fx_forward_review_read(review, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.beforeRevision == 3U && snapshot.afterRevision == 4U && snapshot.recordCount == 2U);
    CHECK(snapshot.before.state == UMI_OPEN_TMS_FX_FORWARD_DRAFT && snapshot.before.version == 1U);
    CHECK(snapshot.after.state == UMI_OPEN_TMS_FX_FORWARD_CONFIRMED && snapshot.after.version == 2U);
    if (!strcmp(mode, "apply") || !strcmp(mode, "equivalent") ||
        !strcmp(mode, "snapshot-isolation") || !strcmp(mode, "unused-bytes")) {
        if (!strcmp(mode, "equivalent")) { free(book); book = malloc(sizeof *book); CHECK(book != NULL); memcpy(book, before, sizeof *book); }
        if (!strcmp(mode, "snapshot-isolation")) {
            snapshot.after.state = UMI_OPEN_TMS_FX_FORWARD_SETTLED;
            snapshot.after.version = 99U; snapshot.after.baseNotional = 99999.0;
        }
        if (!strcmp(mode, "unused-bytes")) {
            memset(&book->trades[book->count], 0xCC, sizeof book->trades[0]);
            book->trades[0].tradeId[63] = 'z';
        }
        CHECK(umi_open_tms_fx_forward_review_apply(book, review) == UMI_STATUS_OK);
        CHECK(book->revision == 4U && book->trades[0].version == 2U);
        CHECK(book->trades[0].state == UMI_OPEN_TMS_FX_FORWARD_CONFIRMED && book->trades[0].baseNotional == 1000.0);
        CHECK(book->trades[1].state == UMI_OPEN_TMS_FX_FORWARD_DRAFT);
        memcpy(before, book, sizeof *book);
        CHECK(umi_open_tms_fx_forward_review_apply(book, review) == UMI_STATUS_BUSY);
        Same(book, before);
    } else if (!strcmp(mode, "source-lifetime")) {
        free(book); book = NULL;
        memset(&snapshot, 0, sizeof snapshot);
        CHECK(umi_open_tms_fx_forward_review_read(review, &snapshot) == UMI_STATUS_OK);
        CHECK(strcmp(snapshot.before.accountId, "treasury-account") == 0);
        CHECK(snapshot.after.version == 2U && snapshot.recordCount == 2U);
    } else if (!strncmp(mode, "changed-", 8U)) {
        UmiOpenTmsFxForward *row = &book->trades[0];
        if (!strcmp(mode, "changed-id")) strcpy(row->tradeId, "other");
        else if (!strcmp(mode, "changed-account")) strcpy(row->accountId, "other");
        else if (!strcmp(mode, "changed-party")) strcpy(row->counterpartyId, "other");
        else if (!strcmp(mode, "changed-base")) strcpy(row->baseCurrency, "EUR");
        else if (!strcmp(mode, "changed-quote")) strcpy(row->quoteCurrency, "EUR");
        else if (!strcmp(mode, "changed-notional")) row->baseNotional += 1.0;
        else if (!strcmp(mode, "changed-rate")) row->forwardRate += 0.01;
        else if (!strcmp(mode, "changed-market")) row->marketForwardRate += 0.01;
        else if (!strcmp(mode, "changed-maturity")) ++row->maturityMilliseconds;
        else if (!strcmp(mode, "changed-state")) row->state = UMI_OPEN_TMS_FX_FORWARD_CANCELLED;
        else if (!strcmp(mode, "changed-version")) ++row->version;
        else if (!strcmp(mode, "changed-revision")) ++book->revision;
        else if (!strcmp(mode, "changed-unrelated")) ++book->trades[1].version;
        else if (!strcmp(mode, "changed-order")) { trade = book->trades[0]; book->trades[0] = book->trades[1]; book->trades[1] = trade; }
        else if (!strcmp(mode, "changed-count")) --book->count;
        else if (!strcmp(mode, "changed-added")) { trade = Trade("trade-3"); CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK); }
        else if (!strcmp(mode, "changed-corrupt")) memset(row->tradeId, 'x', sizeof row->tradeId);
        else if (!strcmp(mode, "changed-nan")) row->baseNotional = NAN;
        else if (!strcmp(mode, "changed-duplicate")) strcpy(book->trades[1].tradeId, row->tradeId);
        else CHECK(0);
        memcpy(before, book, sizeof *book);
        CHECK(umi_open_tms_fx_forward_review_apply(book, review) == UMI_STATUS_BUSY);
        Same(book, before);
    } else if (!strncmp(mode, "refuse-", 7U)) {
        const char *id = "trade-1";
        UmiOpenTmsFxForwardState next = UMI_OPEN_TMS_FX_FORWARD_CONFIRMED;
        uint64_t version = 2U;
        UmiStatus expected = UMI_STATUS_INVALID_STATE;
        if (!strcmp(mode, "refuse-skip")) next = UMI_OPEN_TMS_FX_FORWARD_SETTLED;
        else if (!strcmp(mode, "refuse-version")) version = 1U;
        else if (!strcmp(mode, "refuse-missing")) { id = "missing"; expected = UMI_STATUS_NOT_FOUND; }
        else if (!strcmp(mode, "refuse-limit")) { book->revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
        else if (!strcmp(mode, "refuse-count")) book->count = UMI_OPEN_TMS_FX_FORWARD_CAPACITY + 1U;
        else if (!strcmp(mode, "refuse-duplicate")) strcpy(book->trades[1].tradeId, "trade-1");
        else CHECK(0);
        memcpy(before, book, sizeof *book);
        UmiOpenTmsFxForwardReview *out = review;
        CHECK(umi_open_tms_fx_forward_review_create(book, id, next, version, &out) == expected);
        CHECK(out == NULL); Same(book, before);
    } else if (!strcmp(mode, "invalid-api")) {
        UmiOpenTmsFxForwardReview *out = review;
        CHECK(umi_open_tms_fx_forward_review_create(NULL, "trade-1", UMI_OPEN_TMS_FX_FORWARD_CONFIRMED, 2U, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(out == NULL);
        CHECK(umi_open_tms_fx_forward_review_create(book, NULL, UMI_OPEN_TMS_FX_FORWARD_CONFIRMED, 2U, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(out == NULL);
        CHECK(umi_open_tms_fx_forward_review_create(book, "trade-1", UMI_OPEN_TMS_FX_FORWARD_CONFIRMED, 2U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_open_tms_fx_forward_review_read(review, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        UmiOpenTmsFxForwardReviewSnapshot old;
        memcpy(&old, &snapshot, sizeof old);
        CHECK(umi_open_tms_fx_forward_review_read(NULL, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&old, &snapshot, sizeof old) == 0);
        CHECK(umi_open_tms_fx_forward_review_apply(NULL, review) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_open_tms_fx_forward_review_apply(book, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        Same(book, before);
        umi_open_tms_fx_forward_review_destroy(NULL);
    } else CHECK(0);
    umi_open_tms_fx_forward_review_destroy(review);
    free(before); free(book);
    return 0;
}
