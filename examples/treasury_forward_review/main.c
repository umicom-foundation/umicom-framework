/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/treasury_forward_review/main.c
 *
 * PURPOSE:
 *   Demonstrate local treasury review and refusal of changed observations through the SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/open_tms/fx_forward_review.h"
#include "umicom/open_tms/fx_forward_archive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(EXIT_FAILURE); } } while (0)
int main(int argc, char **argv) {
    /* Retain the original two-scenario entry contract for source review.
     * Archive scenarios now exercise the same installed finance boundary. */
#if 0
    REQUIRE(argc == 2 && (!strcmp(argv[1], "apply") || !strcmp(argv[1], "stale")));
#endif
    REQUIRE(argc == 2 && (!strcmp(argv[1], "apply") || !strcmp(argv[1], "stale") ||
        !strcmp(argv[1], "archive-roundtrip") || !strcmp(argv[1], "archive-stale")));
    UmiOpenTmsFxForwardBook *book = malloc(sizeof *book);
    REQUIRE(book != NULL);
    umi_open_tms_fx_forward_book_init(book);
    UmiOpenTmsFxForward trade = {0};
    strcpy(trade.tradeId, "example-forward"); strcpy(trade.accountId, "practice");
    strcpy(trade.counterpartyId, "synthetic-party");
    strcpy(trade.baseCurrency, "GBP"); strcpy(trade.quoteCurrency, "USD");
    trade.baseNotional = 1000.0; trade.forwardRate = 1.25;
    trade.marketForwardRate = 1.25; trade.version = 1U;
    REQUIRE(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
    if (!strcmp(argv[1], "archive-roundtrip") || !strcmp(argv[1], "archive-stale")) {
        size_t length = 0U;
        REQUIRE(umi_open_tms_fx_forward_book_archive_encode(book, NULL, 0U, &length) == UMI_STATUS_OK);
        unsigned char *bytes = malloc(length);
        UmiOpenTmsFxForwardBook *loaded = malloc(sizeof *loaded), *expected = malloc(sizeof *expected);
        REQUIRE(bytes != NULL && loaded != NULL && expected != NULL);
        REQUIRE(umi_open_tms_fx_forward_book_archive_encode(book, bytes, length, &length) == UMI_STATUS_OK);
        umi_open_tms_fx_forward_book_init(loaded);
        memcpy(expected, loaded, sizeof *expected);
        if (!strcmp(argv[1], "archive-stale")) ++loaded->revision;
        UmiStatus status = umi_open_tms_fx_forward_book_archive_decode_if_unchanged(bytes, length, expected, loaded);
        if (!strcmp(argv[1], "archive-stale")) {
            REQUIRE(status == UMI_STATUS_INVALID_STATE && loaded->count == 0U && loaded->revision == 2U);
            puts("Changed destination refused; the archive did not overwrite it.");
        } else {
            REQUIRE(status == UMI_STATUS_OK && loaded->count == 1U && loaded->trades[0].forwardRate == trade.forwardRate);
            REQUIRE(loaded->trades[0].state == UMI_OPEN_TMS_FX_FORWARD_DRAFT);
            puts("Portable values restored into a separate local draft; no command executed.");
        }
        free(expected); free(loaded); free(bytes); free(book);
        puts("PASS: treasury_forward_review");
        return EXIT_SUCCESS;
    }
    UmiOpenTmsFxForwardReview *review = NULL;
    REQUIRE(umi_open_tms_fx_forward_review_create(book, trade.tradeId,
        UMI_OPEN_TMS_FX_FORWARD_CONFIRMED, 2U, &review) == UMI_STATUS_OK);
    UmiOpenTmsFxForwardReviewSnapshot shown;
    REQUIRE(umi_open_tms_fx_forward_review_read(review, &shown) == UMI_STATUS_OK);
    REQUIRE(shown.before.state == UMI_OPEN_TMS_FX_FORWARD_DRAFT);
    REQUIRE(shown.after.state == UMI_OPEN_TMS_FX_FORWARD_CONFIRMED);
    if (!strcmp(argv[1], "stale")) {
        /* Even an application edit that omits a revision increment invalidates
         * the observation. Production callers should use one serial owner. */
        book->trades[0].marketForwardRate = 1.26;
        REQUIRE(umi_open_tms_fx_forward_review_apply(book, review) == UMI_STATUS_BUSY);
        REQUIRE(book->trades[0].state == UMI_OPEN_TMS_FX_FORWARD_DRAFT);
        puts("Changed observation refused; prepare and inspect a fresh review.");
    } else {
        REQUIRE(umi_open_tms_fx_forward_review_apply(book, review) == UMI_STATUS_OK);
        REQUIRE(book->trades[0].state == UMI_OPEN_TMS_FX_FORWARD_CONFIRMED);
        REQUIRE(umi_open_tms_fx_forward_review_apply(book, review) == UMI_STATUS_BUSY);
        puts("Local lifecycle updated once; no bank or settlement was contacted.");
    }
    umi_open_tms_fx_forward_review_destroy(review);
    free(book);
    puts("PASS: treasury_forward_review");
    return EXIT_SUCCESS;
}
