/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_fx_forward_archive.c
 *
 * PURPOSE:
 *   Exercise portable treasury archive admission, exact values and failure atomicity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/open_tms/fx_forward_archive.h"
#include "../../src/base/value_archive_internal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static UmiOpenTmsFxForward Trade(const char *id) {
    UmiOpenTmsFxForward trade = {0};
    strcpy(trade.tradeId, id); strcpy(trade.accountId, "account"); strcpy(trade.counterpartyId, "party");
    strcpy(trade.baseCurrency, "GBP"); strcpy(trade.quoteCurrency, "USD");
    trade.baseNotional = 1000.25; trade.forwardRate = 1.1; trade.marketForwardRate = 0.0;
    trade.maturityMilliseconds = UINT64_MAX; trade.version = UINT64_MAX;
    trade.state = UMI_OPEN_TMS_FX_FORWARD_SETTLEMENT_PENDING;
    return trade;
}
static size_t Numeric(const unsigned char *bytes, size_t row) {
    for (unsigned field = 0U; field < 5U; ++field)
        row += 8U + (size_t)UmiArchiveLoadInteger(bytes + row, 8U);
    return row;
}
static void Reseal(unsigned char *bytes, size_t size) {
    UmiArchiveStoreInteger(bytes + 24U, UmiArchiveChecksum(bytes, size), 4U);
}
static void Same(const void *a, const void *b, size_t size) { CHECK(memcmp(a, b, size) == 0); }
int main(int argc, char **argv) {
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiOpenTmsFxForwardBook *book = malloc(sizeof *book), *out = malloc(sizeof *out);
    UmiOpenTmsFxForwardBook *scratch = malloc(sizeof *scratch), *before = malloc(sizeof *before);
    CHECK(book && out && scratch && before);
    umi_open_tms_fx_forward_book_init(book);
    UmiOpenTmsFxForward trade = Trade("aa");
    if (strcmp(mode, "empty")) {
        CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
        trade = Trade("bb"); trade.state = UMI_OPEN_TMS_FX_FORWARD_CANCELLED;
        CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
    }
    if (!strcmp(mode, "full")) {
        for (size_t i = book->count; i < UMI_OPEN_TMS_FX_FORWARD_CAPACITY; ++i) {
            (void)snprintf(trade.tradeId, sizeof trade.tradeId, "row-%zu", i);
            CHECK(umi_open_tms_fx_forward_book_add(book, &trade) == UMI_STATUS_OK);
        }
    }
    size_t length = 0U;
    CHECK(umi_open_tms_fx_forward_book_archive_encode(book, NULL, 0U, &length) == UMI_STATUS_OK);
    unsigned char *bytes = malloc(length + 1U), *copy = malloc(length + 1U);
    CHECK(bytes && copy);
    memset(bytes, 0xAA, length + 1U);
    CHECK(umi_open_tms_fx_forward_book_archive_encode(book, bytes, length, &length) == UMI_STATUS_OK);
    memcpy(copy, bytes, length + 1U);
    memset(out, 0xA5, sizeof *out); memcpy(before, out, sizeof *out);
    if (!strcmp(mode, "roundtrip") || !strcmp(mode, "empty") || !strcmp(mode, "full") || !strcmp(mode, "workspace")) {
        UmiStatus status = !strcmp(mode, "workspace")
            ? umi_open_tms_fx_forward_book_archive_decode_with_workspace(bytes, length, scratch, out)
            : umi_open_tms_fx_forward_book_archive_decode(bytes, length, out);
        CHECK(status == UMI_STATUS_OK && out->count == book->count && out->revision == book->revision);
        CHECK(umi_open_tms_fx_forward_book_validate(out) == UMI_STATUS_OK);
        size_t written = 0U;
        CHECK(umi_open_tms_fx_forward_book_archive_encode(out, copy, length, &written) == UMI_STATUS_OK);
        CHECK(written == length); Same(bytes, copy, length);
        if (out->count != 0U) {
            CHECK(out->trades[0].maturityMilliseconds == UINT64_MAX && out->trades[0].version == UINT64_MAX);
            CHECK(out->trades[0].state == UMI_OPEN_TMS_FX_FORWARD_SETTLEMENT_PENDING);
        }
    } else if (!strcmp(mode, "short")) {
        memset(copy, 0xA5, length + 1U);
        size_t needed = 0U;
        CHECK(umi_open_tms_fx_forward_book_archive_encode(book, copy, length - 1U, &needed) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(needed == length);
        for (size_t i = 0U; i < length + 1U; ++i) CHECK(copy[i] == 0xA5U);
    } else if (!strcmp(mode, "tails")) {
        book->trades[0].tradeId[63] = 'x';
        memset(&book->trades[2], 0xCC, sizeof book->trades[2]);
        size_t written = 0U;
        CHECK(umi_open_tms_fx_forward_book_archive_encode(book, copy, length, &written) == UMI_STATUS_OK);
        CHECK(written == length); Same(bytes, copy, length);
        CHECK(umi_open_tms_fx_forward_book_archive_decode(bytes, length, out) == UMI_STATUS_OK);
        CHECK(out->trades[0].tradeId[63] == '\0' && out->trades[2].tradeId[0] == '\0');
    } else if (!strcmp(mode, "truncation")) {
        for (size_t count = 0U; count < length; ++count) {
            CHECK(umi_open_tms_fx_forward_book_archive_decode(bytes, count, out) != UMI_STATUS_OK);
            Same(out, before, sizeof *out);
        }
    } else if (!strcmp(mode, "conditional") || !strcmp(mode, "stale") || !strcmp(mode, "conditional-invalid")) {
        umi_open_tms_fx_forward_book_init(out);
        memcpy(before, out, sizeof *out);
        if (!strcmp(mode, "stale")) { ++out->revision; memcpy(scratch, out, sizeof *out); }
        if (!strcmp(mode, "conditional-invalid")) bytes[32] ^= 1U;
        UmiStatus status = umi_open_tms_fx_forward_book_archive_decode_if_unchanged(bytes, length, before, out);
        if (!strcmp(mode, "conditional")) CHECK(status == UMI_STATUS_OK && out->count == 2U);
        else { CHECK(status != UMI_STATUS_OK); Same(out, !strcmp(mode, "stale") ? scratch : before, sizeof *out); }
    } else if (!strcmp(mode, "overlap")) {
        CHECK(umi_open_tms_fx_forward_book_archive_decode_with_workspace(bytes, length, out, out) == UMI_STATUS_INVALID_ARGUMENT);
        Same(out, before, sizeof *out);
        CHECK(length < sizeof *scratch); memcpy(scratch, bytes, length);
        CHECK(umi_open_tms_fx_forward_book_archive_decode_with_workspace(scratch, length, scratch, out) == UMI_STATUS_INVALID_ARGUMENT);
        Same(out, before, sizeof *out);
    } else if (!strcmp(mode, "invalid-api")) {
        size_t size = 55U;
        CHECK(umi_open_tms_fx_forward_book_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_open_tms_fx_forward_book_archive_encode(NULL, copy, length, &size) == UMI_STATUS_INVALID_ARGUMENT && size == 55U);
        CHECK(umi_open_tms_fx_forward_book_archive_encode(book, NULL, 1U, &size) == UMI_STATUS_INVALID_ARGUMENT && size == 55U);
        CHECK(umi_open_tms_fx_forward_book_archive_decode(NULL, length, out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_open_tms_fx_forward_book_archive_decode_with_workspace(bytes, length, NULL, out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_open_tms_fx_forward_book_archive_decode_if_unchanged(bytes, length, NULL, out) == UMI_STATUS_INVALID_ARGUMENT);
        Same(out, before, sizeof *out);
    } else if (!strncmp(mode, "encode-", 7U)) {
        if (!strcmp(mode, "encode-count")) book->count = UMI_OPEN_TMS_FX_FORWARD_CAPACITY + 1U;
        else if (!strcmp(mode, "encode-text")) memset(book->trades[0].tradeId, 'x', sizeof book->trades[0].tradeId);
        else if (!strcmp(mode, "encode-nan")) book->trades[0].baseNotional = NAN;
        else CHECK(0);
        size_t size = 55U;
        CHECK(umi_open_tms_fx_forward_book_archive_encode(book, copy, length, &size) != UMI_STATUS_OK);
        CHECK(size == 55U); Same(bytes, copy, length);
    } else {
        size_t numeric = Numeric(bytes, 48U), count = length;
        if (!strcmp(mode, "checksum")) bytes[32] ^= 1U;
        else if (!strcmp(mode, "schema")) { bytes[8] ^= 1U; Reseal(bytes, length); }
        else if (!strcmp(mode, "trailing")) { bytes[length] = 1U; ++count; }
        else {
            if (!strcmp(mode, "count")) UmiArchiveStoreInteger(bytes + 40U, UMI_OPEN_TMS_FX_FORWARD_CAPACITY + 1U, 8U);
            else if (!strcmp(mode, "revision")) UmiArchiveStoreInteger(bytes + 32U, 0U, 8U);
            else if (!strcmp(mode, "text-length")) UmiArchiveStoreInteger(bytes + 48U, UINT64_MAX, 8U);
            else if (!strcmp(mode, "text-nul")) bytes[56] = 0U;
            else if (!strcmp(mode, "duplicate")) memcpy(bytes + numeric + 48U + 8U, "aa", 2U);
            else if (!strcmp(mode, "state")) UmiArchiveStoreInteger(bytes + numeric + 32U, 99U, 8U);
            else if (!strcmp(mode, "nan")) UmiArchiveStoreInteger(bytes + numeric, UINT64_C(0x7ff8000000000000), 8U);
            else if (!strcmp(mode, "negative")) UmiArchiveStoreInteger(bytes + numeric, UINT64_C(0xbff0000000000000), 8U);
            else CHECK(0);
            Reseal(bytes, length);
        }
        CHECK(umi_open_tms_fx_forward_book_archive_decode(bytes, count, out) != UMI_STATUS_OK);
        Same(out, before, sizeof *out);
        CHECK(umi_open_tms_fx_forward_book_archive_decode_with_workspace(bytes, count, scratch, out) != UMI_STATUS_OK);
        Same(out, before, sizeof *out);
    }
    free(copy); free(bytes); free(before); free(scratch); free(out); free(book);
    return 0;
}
